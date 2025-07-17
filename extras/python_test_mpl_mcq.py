import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Patch
import interpreter_module
import json
import numpy as np
import sys
import os

# Constants for grid cell size and margin
CELL_SIZE = 1  # Cell size in data coordinates
MARGIN = 0.05  # Margin between cells

# Default color for unknown entities
DEFAULT_COLOR = "black"

# Define the color mapping for different grid entities
COLOR_MAP = {
    "gray": "gray",
    "grey": "grey",
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
    "orangered": "orangered",
    "skyblue": "skyblue",
    "lightcyan": "lightcyan",
    "lightgreen": "lightgreen",
    "magenta": "magenta",
    "darkgray": "darkgray",
    "darkgrey": "darkgrey",
    "darkblue": "darkblue",
    "darkgreen": "darkgreen",
    "darkred": "darkred",
    "black": "black",
    "tan": "tan",
    "lightpink": "lightpink",
    # Add more mappings as needed
}

# Add after existing constants
MASKED_CELL_VALUE = "white"  # Value to use for masked cells


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
        self.ax.set_xlim(-1, 20)  # More generous initial limits
        self.ax.set_ylim(-1, 20)
        self.ax.set_facecolor("black")  # Set axes background to black
        plt.ion()  # Enable interactive mode
        plt.show(block=False)  # Show plot without blocking
        self.rectangles = {}  # Store rectangles for potential updates
        
        # Add new instance variables
        self.masking_mode = False
        self.masked_cells = set()  # Store (x,y) coordinates of cells to mask
        self.frame_history = []  # Store all frame states for saving
        self.action_history = []  # Store actions and their indices
        self.incorrect_option_frames = []  # Store frames to be used as incorrect options
        
        # Add frame navigation variables
        self.current_frame_idx = 0
        
        # Add text display for frame counter
        self.frame_counter = self.ax.text(
            0.02, 0.98, 'Frame: 0',
            transform=self.ax.transAxes,
            color='white',
            fontsize=10,
            verticalalignment='top'
        )
        self.frame_count = 0
        
        self.fig.canvas.mpl_connect("button_press_event", self.on_click)
        self.fig.canvas.mpl_connect("key_press_event", self.on_key_press)

    def on_click(self, event):
        """
        Handles mouse click events on the grid.
        """
        if event.inaxes != self.ax:
            return

        # Calculate grid coordinates
        x_click = event.xdata
        y_click = event.ydata
        
        col = int((x_click - MARGIN) // (CELL_SIZE + MARGIN)) 
        row = int((self.grid_size - y_click - MARGIN) // (CELL_SIZE + MARGIN)) + 1

        if 0 <= col < self.grid_size and 0 <= row < self.grid_size:
            if self.masking_mode:
                # Toggle cell masking
                cell_coord = (col, row)
                if cell_coord in self.masked_cells:
                    self.masked_cells.remove(cell_coord)
                else:
                    self.masked_cells.add(cell_coord)
                print(f"Toggled mask at Row: {row}, Col: {col}")
                self.render()
            else:
                # Normal click handling
                print(f"Grid clicked at Row: {row}, Col: {col}")
                self.interpreter.click(col, row)
                self.interpreter.step()
                # Store click action
                self.action_history.append({
                    'action': 'click',
                    'idx': [row, col]
                })

                self.store_frame()  # Store the frame with the current action
        else:
            print("Click was outside the grid boundaries.")

    def on_key_press(self, event):
        """
        Handles key press events.
        """
        key = event.key
        print(f"Key pressed: {key}")

        if key == "n":  # Next frame
            self.frame_count += 1
            self.interpreter.step()
            self.action_history.append({
                'action': 'None',
                'idx': 'None'
            })
            self.store_frame()  # Store the frame with the current action
            self.render()
        elif key == "m":  # Toggle masking mode
            self.masking_mode = not self.masking_mode
            print(f"Masking mode {'enabled' if self.masking_mode else 'disabled'}")
        elif key == "c":  # Clear all masked cells
            self.masked_cells.clear()
            self.render()
        elif key == "s":  # Save frames to JSON
            self.save_frames_to_json()
        elif key == "i":  # Save current frame as incorrect option
            if self.grid:
                frame_data = {
                    'original': self.grid.copy(),
                    'grid_size': self.grid_size,
                    'masked_cells': list(self.masked_cells),
                    'action': self.action_history[-1]['action'] if self.action_history else 'None',
                    'idx': self.action_history[-1]['idx'] if self.action_history else 'None'
                }
                self.incorrect_option_frames.append(frame_data)
                print(f"Saved frame {len(self.incorrect_option_frames)} as incorrect option")
        elif key == "q":
            print("Quitting the game.")
            plt.close(self.fig)
            sys.exit()
        elif key == "left":
            self.interpreter.left()
            print("Pushed Left.")
            self.action_history.append({
                'action': 'left',
                'idx': 'None'
            })
            self.store_frame()  # Store the frame with the current action
            self.render()
        elif key == "right":
            self.interpreter.right()
            print("Pushed Right.")
            self.action_history.append({
                'action': 'right',
                'idx': 'None'
            })
            self.store_frame()  # Store the frame with the current action
            self.render()
        elif key == "up":
            self.interpreter.up()
            print("Pushed Up.")
            self.action_history.append({
                'action': 'up',
                'idx': 'None'
            })
            self.store_frame()  # Store the frame with the current action
            self.render()
        elif key == "down":
            self.interpreter.down()
            print("Pushed Down.")
            self.action_history.append({
                'action': 'down',
                'idx': 'None'
            })
            self.store_frame()  # Store the frame with the current action
            self.render()
        else:
            print(f"Unhandled key: {key}")

    def renderer_render(self):
        """
        Helper method to render after an action.
        """
        self.render()

    def create_masked_grid(self, original_grid):
        """
        Creates a copy of the grid with masked cells replaced with gray.
        """
        masked_grid = json.loads(json.dumps(original_grid))  # Deep copy
        
        for x, y in self.masked_cells:
            # Find and modify the cell in the grid
            for elem in masked_grid:
                for subelem in masked_grid[elem][:]:  # Create a copy to iterate
                    if (subelem["position"]["x"] == x and 
                        subelem["position"]["y"] == y):
                        subelem["color"] = MASKED_CELL_VALUE
        
        return masked_grid

    def save_frames_to_json(self):
        """
        Saves all frames to a JSON file in the new format.
        """
        # Add current frame to history if not empty
        if self.grid:
            self.store_frame()  # Store the current frame with its action

        # Get the script content and name from the file
        script = ""
        name = "test2"  # Default name
        if len(sys.argv) > 1:
            script_path = sys.argv[1]
            try:
                with open(script_path, "r") as f:
                    script = f.read()
                # Get the name from the filename without extension
                name = os.path.splitext(os.path.basename(script_path))[0]
            except FileNotFoundError:
                print(f"Script file {script_path} not found.")
        
        # Get all unique colors from the frames
        unique_colors = set()
        for frame in self.frame_history:
            for elem in frame['original']:
                for subelem in frame['original'][elem]:
                    unique_colors.add(subelem["color"].lower())
        
        # Update COLOR_MAP with any new colors
        for color in unique_colors:
            if color not in COLOR_MAP:
                # Add new color to COLOR_MAP with a default value
                COLOR_MAP[color] = color
        
        # Create colors dictionary for JSON output
        colors = {
            "0": {"g":0.0,"alpha":0.0,"b":0.0,"r":0.0},  # Empty cell
            "1": {"g":0.05,"alpha":1.0,"b":0.05,"r":0.05}  # Background
        }
        
        # Add colors from COLOR_MAP with numeric keys
        for i, color in enumerate(COLOR_MAP.keys(), start=2):
            # Get RGB values from matplotlib's color palette
            rgb = plt.cm.colors.to_rgb(color)
            colors[str(i)] = {
                "r": rgb[0],
                "g": rgb[1],
                "b": rgb[2],
                "alpha": 0.8
            }
        
        # Create output structure
        output = {
            "eval_type": "untimed",
            "name": name,
            "program": script,
            "colors": colors,
            "questions": [
                {
                    "id": 1,
                    "observations": [],
                    "incorrect_options": []
                }
            ]
        }

        # Convert frames to observations
        for i, frame in enumerate(self.frame_history):
            # Create empty grid with proper dimensions
            grid_size = frame['grid_size']
            grid = [[1 for _ in range(grid_size)] for _ in range(grid_size)]
            mask = [[1 for _ in range(grid_size)] for _ in range(grid_size)]
            
            # Fill in particles
            for elem in frame['original']:
                for subelem in frame['original'][elem]:
                    x = subelem["position"]["x"]
                    y = subelem["position"]["y"]
                    # Ensure x and y are within grid bounds
                    if 0 <= x < grid_size and 0 <= y < grid_size:
                        color = subelem["color"].lower()
                        # Map color to numeric value using COLOR_MAP
                        color_idx = list(COLOR_MAP.keys()).index(color) + 2 if color in COLOR_MAP else 1
                        grid[y][x] = color_idx

            # Fill in mask
            for x, y in frame['masked_cells']:
                if 0 <= x < grid_size and 0 <= y < grid_size:
                    mask[y][x] = 0

            # Get action from frame data
            action = frame.get('action', 'None')
            idx = frame.get('idx', 'None')

            observation = {
                "time_step": i + 1,
                "grid": grid,
                "action": action,
                "idx": idx,
                "mask": mask
            }
            output["questions"][0]["observations"].append(observation)

        # Create incorrect options from saved incorrect option frames
        incorrect_options = []
        for frame in self.incorrect_option_frames:
            grid_size = frame['grid_size']
            grid = [[1 for _ in range(grid_size)] for _ in range(grid_size)]
            
            # Fill in particles
            for elem in frame['original']:
                for subelem in frame['original'][elem]:
                    x = subelem["position"]["x"]
                    y = subelem["position"]["y"]
                    if 0 <= x < grid_size and 0 <= y < grid_size:
                        color = subelem["color"].lower()
                        color_idx = list(COLOR_MAP.keys()).index(color) + 2 if color in COLOR_MAP else 1
                        grid[y][x] = color_idx
            
            # Only include masked cells in the incorrect option
            masked_section = []
            for y in range(grid_size):
                row = []
                has_masked_cell = False
                for x in range(grid_size):
                    if (x, y) in frame['masked_cells']:
                        row.append(grid[y][x])
                        has_masked_cell = True
                if has_masked_cell:
                    masked_section.append(row)
            
            if masked_section:
                incorrect_options.append(masked_section)
        
        # If we have at least 3 incorrect options, use them
        if len(incorrect_options) >= 3:
            output["questions"][0]["incorrect_options"] = incorrect_options[:3]
        else:
            # If we have fewer than 3 incorrect options, use the ones we have and fill the rest with the last frame from history
            last_frame = self.frame_history[-1] if self.frame_history else None
            if last_frame:
                # Create a grid from the last frame
                grid_size = last_frame['grid_size']
                grid = [[1 for _ in range(grid_size)] for _ in range(grid_size)]
                
                # Fill in particles from the last frame
                for elem in last_frame['original']:
                    for subelem in last_frame['original'][elem]:
                        x = subelem["position"]["x"]
                        y = subelem["position"]["y"]
                        if 0 <= x < grid_size and 0 <= y < grid_size:
                            color = subelem["color"].lower()
                            color_idx = list(COLOR_MAP.keys()).index(color) + 2 if color in COLOR_MAP else 1
                            grid[y][x] = color_idx
                
                # Create a masked section from the last frame
                masked_section = []
                for y in range(grid_size):
                    row = []
                    has_masked_cell = False
                    for x in range(grid_size):
                        if (x, y) in last_frame['masked_cells']:
                            row.append(grid[y][x])
                            has_masked_cell = True
                    if has_masked_cell:
                        masked_section.append(row)
                
                # Fill in the remaining options with the last frame's masked section
                while len(incorrect_options) < 3:
                    incorrect_options.append(masked_section)
            
            output["questions"][0]["incorrect_options"] = incorrect_options

        # Create output_json directory if it doesn't exist
        output_dir = "output_json"
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        # Save to file in the output_json folder
        try:
            # Get the program file name from sys.argv
            if len(sys.argv) > 1:
                program_file = sys.argv[1]
                json_file = os.path.join(output_dir, os.path.basename(program_file).replace('.sexp', '.json'))
            else:
                json_file = os.path.join(output_dir, 'frames.json')
            
            # Write JSON output with indentation
            with open(json_file, 'w') as f:
                json.dump(output, f, indent=4)
            print(f"Saved {len(output['questions'][0]['observations'])} frames to {json_file}")
        except Exception as e:
            print(f"Error saving frames: {e}")

    def load_frame(self, frame_idx):
        """
        Loads and displays a specific frame from history.
        """
        if 0 <= frame_idx < len(self.frame_history):
            frame = self.frame_history[frame_idx]
            self.grid = frame['original'].copy()
            self.grid_size = frame['grid_size']
            self.masked_cells = set(tuple(cell) for cell in frame['masked_cells'])
            print(f"Loaded frame {frame_idx + 1}/{len(self.frame_history)}")
            self.render()

    def update_frame_counter(self):
        """
        Updates the frame counter display.
        """
        total_frames = len(self.frame_history)
        current_frame = self.current_frame_idx + 1 if total_frames > 0 else 0
        self.frame_counter.set_text(f'Frame: {current_frame}/{total_frames}')

    def store_frame(self):
        """
        Stores current frame state in history.
        """
        # Get the current action if it exists, otherwise use "None"
        current_action = "None"
        current_idx = "None"
        if self.action_history:
            current_action = self.action_history[-1]['action']
            current_idx = self.action_history[-1]['idx']

        frame_data = {
            'original': self.grid.copy(),
            'grid_size': self.grid_size,
            'masked_cells': list(self.masked_cells),
            'action': current_action,
            'idx': current_idx
        }
        self.frame_history.append(frame_data)
        print(f"Stored frame {self.frame_count} with action {current_action}")

    def render(self):
        """
        Renders or updates the grid on the axes without blocking.
        """
        render_output = self.interpreter.render_all()
        grid, grid_size = parse_grid(render_output)
        self.grid = grid
        self.grid_size = grid_size

        # Clear existing patches and reset axes
        self.ax.cla()
        self.ax.set_aspect("equal")
        self.ax.set_axis_off()
        self.ax.set_facecolor("black")  # Ensure axes background remains black

        # Calculate total grid size including margins
        total_size = grid_size * (CELL_SIZE + MARGIN) + MARGIN
        
        # Update plot limits to accommodate the entire grid with margins
        self.ax.set_xlim(-MARGIN, total_size)
        self.ax.set_ylim(-MARGIN, total_size)

        # Debug: Print grid and grid_size
        print("\nGrid Information:")
        print(f"    Grid Size: {grid_size}")
        print("    Grid Data:")
        for elem in grid:
            print(f"        {elem}:")
            for subelem in grid[elem]:
                print(f"            {subelem}")

        # Clear existing rectangles
        self.rectangles.clear()

        for elem in grid:
            for subelem in grid[elem]:
                col_idx = subelem["position"]["x"]
                row_idx = subelem["position"]["y"]
                color_key = subelem["color"].lower()
                color = COLOR_MAP.get(color_key, color_key)
                print(
                    f"    Drawing cell at (Row: {row_idx}, Col: {col_idx}) with color: {color}"
                )

                # Calculate positions with proper margin spacing
                x = MARGIN + col_idx * (CELL_SIZE + MARGIN)
                y = MARGIN + (self.grid_size - row_idx - 1) * (CELL_SIZE + MARGIN)

                # Create and add rectangle
                rect = Rectangle(
                    (x, y), CELL_SIZE, CELL_SIZE, facecolor=color, edgecolor="white"
                )
                self.ax.add_patch(rect)
                self.rectangles[(row_idx, col_idx)] = rect

        # Add visual indicator for masked cells
        for col, row in self.masked_cells:
            x = MARGIN + col * (CELL_SIZE + MARGIN)
            y = MARGIN + (self.grid_size - row - 1) * (CELL_SIZE + MARGIN)
            rect = Rectangle(
                (x, y), CELL_SIZE, CELL_SIZE,
                facecolor='none',  # Transparent fill
                edgecolor='white',  # White border
                linestyle='--',     # Dashed line
                linewidth=2
            )
            self.ax.add_patch(rect)

        # Update frame counter
        self.frame_counter = self.ax.text(
            0.02, 0.98, f'Frame: {self.frame_count}',
            transform=self.ax.transAxes,
            color='white',
            fontsize=10,
            verticalalignment='top'
        )

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
        render_output = interpreter.render_all()
        print(render_output)
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

    print("\nMultiple Choice Question Creation Tool Commands:")
    print("m: Toggle masking mode")
    print("c: Clear all masks")
    print("n: Next frame")
    print("s: Save all frames to JSON")
    print("i: Save current frame as incorrect option")
    print("q: Quit\n")


if __name__ == "__main__":
    main()
