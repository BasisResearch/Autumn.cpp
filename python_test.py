import arcade
import interpreter_module
import json
import sys

# Constants for grid cell size and margin
CELL_SIZE = 50  # Pixels
MARGIN = 5  # Pixels
DEFAULT_COLOR = arcade.color.WHITE

# Define the color mapping for different grid entities
COLOR_MAP = {
    "gray": arcade.color.GRAY,
    "gold": arcade.color.GOLD,
    "green": arcade.color.GREEN,
    "mediumpurple": arcade.color.MEDIUM_PURPLE,
    "purple": arcade.color.PURPLE,
    "white": arcade.color.WHITE,
    "yellow": arcade.color.YELLOW,
    "blue": arcade.color.BLUE,
    "red": arcade.color.RED,
    "orange": arcade.color.ORANGE,
    "darkgreen": arcade.color.DARK_GREEN,
    # Add more mappings as needed
}

UPDATE_INTERVAL = 0.2  # seconds
RENDER_INTERVAL = 0.5  # seconds


class GridRenderer(arcade.Window):
    def __init__(self, interpreter, grid_size):
        super().__init__(
            grid_size * (CELL_SIZE + MARGIN) + MARGIN,
            grid_size * (CELL_SIZE + MARGIN) + MARGIN,
            "Grid Game",
        )
        self.interpreter = interpreter
        self.grid = {}
        self.grid_size = grid_size
        self.sprite_list = arcade.SpriteList()
        self.set_update_rate(1 / 60)  # 60 FPS

    def setup(self):
        self.update_timer = 0
        self.render_timer = 0
        self.process_initial_render()

    def process_initial_render(self):
        render_output = self.interpreter.render_all()
        self.grid, self.grid_size = parse_grid(render_output)
        self.create_sprites()

    def create_sprites(self):
        self.sprite_list = arcade.ShapeElementList()
        for elem in self.grid:
            for subelem in self.grid[elem]:
                col_idx = subelem["position"]["x"]
                row_idx = subelem["position"]["y"]
                color_key = subelem["color"].lower()
                color = COLOR_MAP.get(color_key, DEFAULT_COLOR)

                x = MARGIN + col_idx * (CELL_SIZE + MARGIN) + CELL_SIZE / 2
                y = (
                    MARGIN
                    + (self.grid_size - row_idx - 1) * (CELL_SIZE + MARGIN)
                    + CELL_SIZE / 2
                )

                # Create a colored square as a sprite
                shape = arcade.create_rectangle_filled(
                    x, y, CELL_SIZE, CELL_SIZE, color
                )
                self.sprite_list.append(shape)

    def on_draw(self):
        arcade.start_render()
        self.sprite_list.draw()

    def on_update(self, delta_time):
        self.update_timer += delta_time
        self.render_timer += delta_time

        if self.update_timer >= UPDATE_INTERVAL:
            self.interpreter.step()
            self.update_timer = 0

        if self.render_timer >= RENDER_INTERVAL:
            self.render_grid()
            self.render_timer = 0

    def render_grid(self):
        render_output = self.interpreter.render_all()
        self.grid, self.grid_size = parse_grid(render_output)
        self.create_sprites()

    def on_mouse_press(self, x, y, button, modifiers):
        col = (x - MARGIN) // (CELL_SIZE + MARGIN)
        row = (self.grid_size * (CELL_SIZE + MARGIN) - y - MARGIN) // (
            CELL_SIZE + MARGIN
        )
        if 0 <= col < self.grid_size and 0 <= row < self.grid_size:
            print(f"Grid clicked at Row: {row}, Col: {col}")
            self.interpreter.click(col, row)
            self.render_grid()
        else:
            print("Click was outside the grid boundaries.")

    def on_key_press(self, symbol, modifiers):
        if symbol == arcade.key.LEFT:
            self.interpreter.left()
            self.render_grid()
        elif symbol == arcade.key.RIGHT:
            self.interpreter.right()
            self.render_grid()
        elif symbol == arcade.key.UP:
            self.interpreter.up()
            self.render_grid()
        elif symbol == arcade.key.DOWN:
            self.interpreter.down()
            self.render_grid()
        elif symbol == arcade.key.Q:
            print("Quitting the game.")
            arcade.close_window()
            sys.exit()
        else:
            print(f"Unhandled key: {arcade.key.symbol_string(symbol)}")


def parse_grid(render_output: str):
    """
    Parses the JSON string output from render_all() into a grid and its size.

    Args:
        render_output (str): The JSON string representation of the grid.

    Returns:
        tuple: A tuple containing the grid dictionary and grid size.
    """
    try:
        elem_dict = json.loads(render_output)
    except json.JSONDecodeError as e:
        print(f"JSON Decode Error: {e}")
        return {}, 0
    grid = elem_dict
    grid_size = elem_dict.pop("GRID_SIZE", 0)
    return grid, grid_size


def main():
    # Initialize the Interpreter
    interpreter = interpreter_module.Interpreter()

    # Check for script file argument
    if len(sys.argv) > 1:
        script_path = sys.argv[1]
    else:
        script_path = "./tests/test2.sexp"

    # Read and run the script
    try:
        with open(script_path, "r") as f:
            script = f.read()
        run_result = interpreter.run_script(script)
        print("Run Script Result:", run_result)
    except FileNotFoundError:
        print(f"Script file {script_path} not found.")
        run_result = ""

    # Assume GRID_SIZE is provided after running the script
    render_output = interpreter.render_all()
    grid, grid_size = parse_grid(render_output)

    # Initialize and run the renderer
    window = GridRenderer(interpreter, grid_size)
    window.setup()
    arcade.run()


if __name__ == "__main__":
    main()
