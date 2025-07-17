import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Patch
import interpreter_module
import json
import numpy as np
import sys
import time

# Constants for grid cell size and margin
CELL_SIZE = 1  # Cell size in data coordinates
MARGIN = 0.05  # Margin between cells

# Default color for unknown entities
DEFAULT_COLOR = "black"

# Define the color mapping for different grid entities
COLOR_MAP = {
    "gray": "gray",
    "gold": "gold",
    "green": "green",
    "mediumpurple": "mediumpurple",
    "purple": "purple",
    "white": "white",
    "yellow": "yellow",
    "blue": "blue",
    "red": "red",
    "orange": "orange",
    "sandybrown": "sandybrown",
    "brown": "brown",
    "pink": "pink",
    "lightblue": "lightblue",
    # Add more mappings as needed
}


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
        # print(elem_dict)
    except json.JSONDecodeError as e:
        print(f"JSON Decode Error: {e}")
        return {}, 0
    grid = elem_dict
    grid_size = elem_dict.pop("GRID_SIZE", 0)
    return grid, grid_size


UPDATE_INTERVAL = 100  # 0.3 second
RENDER_INTERVAL = 200  # 0.5 seconds
REBOUNCE = 100  # 0.1 second

# Define the state object
state = {
    "isInteractivePhase": True,
    "lastUpdateTime": 0,
    "lastRenderTime": 0,
}


class GridRenderer:
    def __init__(self, interpreter):
        self.interpreter = interpreter
        self.grid = {}
        self.grid_size = 0
        self.fig, self.ax = plt.subplots(
            figsize=(10, 10), facecolor="black"
        )  # Set figure background to black
        self.ax.set_aspect("equal")
        self.ax.set_axis_off()
        self.ax.set_xlim(-2, 18)  # Initial limits; will be updated
        self.ax.set_ylim(-2, 18)
        self.ax.set_facecolor("black")  # Set axes background to black
        plt.ion()  # Enable interactive mode
        plt.show(block=False)  # Show plot without blocking
        self.rectangles = {}  # Store rectangles for potential updates

        self.fig.canvas.mpl_connect("button_press_event", self.on_click)
        self.fig.canvas.mpl_connect("key_press_event", self.on_key_press)
        self.timer = self.fig.canvas.new_timer(interval=UPDATE_INTERVAL)
        self.timer.add_callback(self.periodic_step)
        self.timer.start()

    def on_click(self, event):
        """
        Handles mouse click events on the grid.

        Args:
            event: Matplotlib mouse event.
        """
        if event.inaxes != self.ax:
            return

        # Calculate grid coordinates based on click position
        x_click = event.xdata
        y_click = event.ydata

        col = int((x_click - MARGIN) // (CELL_SIZE + MARGIN))
        row = int((self.grid_size - y_click - MARGIN) // (CELL_SIZE + MARGIN))

        # Validate grid coordinates
        if 0 <= col < self.grid_size and 0 <= row < self.grid_size:
            print(f"Grid clicked at Row: {row}, Col: {col}")
            self.interpreter.click(col, row)
            self.step_action()
        else:
            print("Click was outside the grid boundaries.")

    def on_key_press(self, event):
        """
        Handles key press events.

        Args:
            event: Matplotlib key event.
        """
        key = event.key
        print(f"Key pressed: {key}")

        if key == "left":
            self.interpreter.left()
            self.step_action()
        elif key == "right":
            self.interpreter.right()
            self.step_action()
        elif key == "up":
            self.interpreter.up()
            self.step_action()
        elif key == "down":
            self.interpreter.down()
            self.step_action()
        elif key == "q":
            print("Quitting the game.")
            plt.close(self.fig)
            sys.exit()
        else:
            print(f"Unhandled key: {key}")

    def step_action(self):
        """
        Performs a step action and renders the updated grid.
        """
        pass

    def periodic_step(self):
        """
        Periodically called step action via Timer.
        """
        start = time.time()
        self.interpreter.step()
        end = time.time()
        print(f"Step time: {end - start}")
        self.render()

    def renderer_render(self):
        """
        Helper method to render after an action.
        """
        self.render()

    def render(self):
        """
        Renders or updates the grid on the axes without blocking.
        """
        render_output = self.interpreter.render_all()
        grid, grid_size = parse_grid(render_output)
        self.grid = grid
        self.grid_size = grid_size

        # Update plot limits based on grid size
        self.ax.set_xlim(0, self.grid_size)
        self.ax.set_ylim(0, self.grid_size)

        # Clear existing patches
        self.ax.cla()
        self.ax.set_aspect("equal")
        self.ax.set_axis_off()
        self.ax.set_xlim(0, self.grid_size)
        self.ax.set_ylim(0, self.grid_size)

        # Debug: Print grid and grid_size
        # print(f"Grid Size: {grid_size}")
        # print(f"Grid Data: {grid}")

        # Clear existing rectangles
        self.rectangles.clear()
        bg_color = self.interpreter.get_background()
        print(f"Background color: {bg_color}")
        self.ax.set_facecolor(bg_color)  # Ensure axes background remains black

        for elem in grid:
            for subelem in grid[elem]:
                col_idx = subelem["position"]["x"]
                row_idx = subelem["position"]["y"]
                color_key = subelem["color"].lower()
                color = COLOR_MAP.get(color_key, color_key)
                # print(
                #     f"Drawing cell at (Row: {row_idx}, Col: {col_idx}) with color: {color}"
                # )

                # Calculate positions
                x = MARGIN + col_idx * (CELL_SIZE + MARGIN)
                y = MARGIN + (self.grid_size - row_idx - 1) * (CELL_SIZE + MARGIN)

                # Create and add rectangle
                rect = Rectangle(
                    (x, y), CELL_SIZE, CELL_SIZE, facecolor=color, edgecolor="white"
                )
                self.ax.add_patch(rect)
                self.rectangles[(row_idx, col_idx)] = rect

        # Create legend
        legend_patches = [
            Patch(
                facecolor=COLOR_MAP[color], edgecolor="white", label=color.capitalize()
            )
            for color in COLOR_MAP
        ]
        legend_patches.append(
            Patch(facecolor=DEFAULT_COLOR, edgecolor="white", label="Default")
        )

        # self.ax.legend(
        #     handles=legend_patches, loc="upper right", bbox_to_anchor=(1.15, 1)
        # )
        # remove legend
        self.ax.legend().remove()

        # Refresh the plot
        self.fig.canvas.draw_idle()
        self.fig.canvas.flush_events()
        plt.pause(0.001)  # Brief pause to allow the plot to update


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
        run_result = interpreter.run_script(script, "", "", 42)
        print("Run Script Result:", run_result)
    except FileNotFoundError:
        print(f"Script file {script_path} not found.")
        run_result = ""

    # Initialize renderer
    renderer = GridRenderer(interpreter)
    renderer.render()

    # Start command loop
    while True:
        try:
            inp = input(
                "Enter command (click x y, step, left, right, up, down, q to quit): "
            )
        except EOFError:
            break  # Exit on EOF (e.g., Ctrl+D)

        if inp.lower() == "q":
            break
        elif inp.lower().startswith("click"):
            parts = inp.split()
            if len(parts) != 3:
                print("Invalid click command. Use: click x y")
                continue
            try:
                _, x, y = parts
                x = int(x)
                y = int(y)
                interpreter.click(x, y)
                print(f"Clicked at ({x}, {y})")
            except ValueError:
                print("Invalid coordinates. x and y should be integers.")
                continue
        elif inp.lower() == "step":
            interpreter.step()
            print("Executed step.")
        elif inp.lower() == "left":
            interpreter.left()
            print("Pushed Left.")
        elif inp.lower() == "right":
            interpreter.right()
            print("Pushed Right.")
        elif inp.lower() == "up":
            interpreter.up()
            print("Pushed Up.")
        elif inp.lower() == "down":
            interpreter.down()
            print("Pushed Down.")
        else:
            print("Unknown command.")
            continue

        # Render after each command
        renderer.render()

    # Cleanup
    plt.ioff()
    plt.show()


if __name__ == "__main__":
    main()
