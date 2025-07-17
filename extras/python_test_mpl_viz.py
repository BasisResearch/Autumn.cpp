import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Patch
import matplotlib.animation as animation
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
    def __init__(self, interpreter, interaction_session=None):
        self.interpreter = interpreter
        self.grid = {}
        self.grid_size = 0
        self.fig, self.ax = plt.subplots(
            figsize=(10, 10), facecolor="black"
        )
        self.ax.set_aspect("equal")
        self.ax.set_axis_off()
        self.ax.set_xlim(-2, 18)
        self.ax.set_ylim(-2, 18)
        self.ax.set_facecolor("black")
        self.rectangles = {}
        self.last_action = "No action yet"
        self.action_text = None
        
        # Store interaction data for animation
        self.interaction_session = interaction_session
        if interaction_session:
            self.interpreter = interpreter_module.Interpreter()
            self.interpreter.run_script(open("tests/" + self.interaction_session['programId'].replace("_dd", "").replace("_ap", "") + ".sexp",'r').read(), "", "")
        self.current_step = 0
        
        # For video recording
        self.frames = []
        
        plt.ion()  # Enable interactive mode
        plt.show(block=False)  # Show plot without blocking

        self.fig.canvas.mpl_connect("button_press_event", self.on_click)
        self.fig.canvas.mpl_connect("key_press_event", self.on_key_press)
        self.timer = self.fig.canvas.new_timer(interval=UPDATE_INTERVAL)
        self.timer.add_callback(self.periodic_step)
        self.timer.start()

        self.clicked_cell = None  # Store the last clicked cell
        self.click_highlight_time = 0  # Track when the cell was clicked
        self.HIGHLIGHT_DURATION = 0.5  # How long to show the highlight (in seconds)

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
            # Store the clicked cell and time
            self.clicked_cell = (row, col)
            self.click_highlight_time = time.time()
            print(f"Set clicked_cell to {self.clicked_cell}")  # Debug print
            self.step_action()
            self.update_action_text(f"Click at ({col}, {row})")
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
            self.update_action_text("Left")
        elif key == "right":
            self.interpreter.right()
            self.step_action()
            self.update_action_text("Right")
        elif key == "up":
            self.interpreter.up()
            self.step_action()
            self.update_action_text("Up")
        elif key == "down":
            self.interpreter.down()
            self.step_action()
            self.update_action_text("Down")
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

        # Clear everything and reset
        self.ax.cla()
        self.ax.set_aspect("equal")
        self.ax.set_axis_off()
        self.ax.set_xlim(0, self.grid_size)
        self.ax.set_ylim(0, self.grid_size)
        self.ax.set_facecolor("black")

        # Clear existing rectangles
        self.rectangles.clear()

        # Check if we should still show the highlight
        current_time = time.time()
        if self.clicked_cell and (current_time - self.click_highlight_time) > self.HIGHLIGHT_DURATION:
            self.clicked_cell = None

        # First draw all base rectangles
        for elem in grid:
            for subelem in grid[elem]:
                col_idx = subelem["position"]["x"]
                row_idx = subelem["position"]["y"]
                color_key = subelem["color"].lower()
                color = COLOR_MAP.get(color_key, color_key)

                # Calculate positions
                x = MARGIN + col_idx * (CELL_SIZE + MARGIN)
                y = MARGIN + (self.grid_size - row_idx - 1) * (CELL_SIZE + MARGIN)

                # Create and add rectangle
                rect = Rectangle(
                    (x, y), CELL_SIZE, CELL_SIZE, 
                    facecolor=color, 
                    edgecolor="white",
                    linewidth=1
                )
                self.ax.add_patch(rect)
                self.rectangles[(row_idx, col_idx)] = rect

        # Then draw highlights on top
        if self.clicked_cell:
            row_idx, col_idx = self.clicked_cell
            x = MARGIN + col_idx * (CELL_SIZE + MARGIN)
            y = MARGIN + (self.grid_size - row_idx - 1) * (CELL_SIZE + MARGIN)
            
            # Add a more visible highlight
            highlight = Rectangle(
                (x, y), CELL_SIZE, CELL_SIZE,
                facecolor='lightblue',
                alpha=0.5,  # Increased opacity
                edgecolor='white',
                linewidth=3,  # Increased linewidth
                zorder=10  # Ensure highlight is drawn on top
            )
            self.ax.add_patch(highlight)
            print(f"Drawing highlight at ({row_idx}, {col_idx})")  # Debug print

        # Add action text
        self.action_text = self.ax.text(
            0.02, 0.98, f"Last action: {self.last_action}",
            transform=self.ax.transAxes,
            color='white',
            fontsize=12,
            fontweight='bold',
            verticalalignment='top',
            bbox=dict(
                facecolor='black',
                alpha=0.8,
                edgecolor='white',
                boxstyle='round,pad=0.5'
            )
        )

        # Refresh the plot
        self.fig.canvas.draw_idle()
        self.fig.canvas.flush_events()
        plt.pause(0.001)

    def update_action_text(self, action):
        """
        Updates the action text display with the latest action.
        
        Args:
            action (str): The action to display
        """
        self.last_action = action
        if self.action_text is None:
            # Create new text object if it doesn't exist
            self.action_text = self.ax.text(
                0.02, 0.98, f"Last action: {action}",
                transform=self.ax.transAxes,
                color='white',
                fontsize=12,  # Increased font size
                fontweight='bold',  # Made text bold
                verticalalignment='top',
                bbox=dict(
                    facecolor='black',
                    alpha=0.8,  # Increased opacity
                    edgecolor='white',
                    boxstyle='round,pad=0.5'  # Added rounded corners
                )
            )
        else:
            # Update existing text object
            self.action_text.set_text(f"Last action: {action}")

    def process_action(self, action):
        """Process a single action from the interaction data"""
        try:
            if action == "reset":
                self.interpreter.run_script(open("tests/" + self.interaction_session['programId'].replace("_dd", "").replace("_ap", "") + ".sexp",'r').read(), "", "")
            elif action == "none":
                pass
            elif action.startswith("click"):
                try:
                    _, x, y = action.split()
                    x, y = int(x), int(y)
                    self.interpreter.click(x, y)
                    # Store the clicked cell and time
                    self.clicked_cell = (y, x)  # Note: y is row, x is column
                    self.click_highlight_time = time.time()
                except Exception as e:
                    print(f"Error processing click action: {e}")
            elif action == "left":
                self.interpreter.left()
            elif action == "right":
                self.interpreter.right()
            elif action == "up":
                self.interpreter.up()
            elif action == "down":
                self.interpreter.down()
            
            self.update_action_text(action)
            self.render()
        except Exception as e:
            print(f"Error in process_action: {e}")

    def animate(self, frame):
        """Animation function for creating the video"""
        if self.current_step < len(self.interaction_session['interactionData']):
            action = self.interaction_session['interactionData'][self.current_step]
            self.process_action(action)
            self.current_step += 1
        return []

    def save_animation(self, filename="animation.mp4"):
        """Save the animation to a video file"""
        anim = animation.FuncAnimation(
            self.fig, 
            self.animate,
            frames=len(self.interaction_session['interactionData']),
            interval=20,  # Reduced to 20ms between frames
            blit=True
        )
        anim.save(filename, writer='ffmpeg', fps=40)  # Increased to 30 fps


def main():
    # Initialize the Interpreter
    interpreter = interpreter_module.Interpreter()

    # Check for script file argument
    if len(sys.argv) > 1 and sys.argv[1].endswith('.sexp'):
        script_path = sys.argv[1]
        # Read and run the script
        try:
            with open(script_path, "r") as f:
                script = f.read()
            run_result = interpreter.run_script(script, "", "")
            print("Run Script Result:", run_result)
        except FileNotFoundError:
            print(f"Script file {script_path} not found.")
            run_result = ""
    else:
        script_path = "./tests/test2.sexp"

    # Read interaction data if provided
    interaction_sessions = None
    if len(sys.argv) > 1 and sys.argv[1].endswith('.json'):
        try:
            with open(sys.argv[1], 'r') as f:
                data = json.load(f)
                if isinstance(data, list):
                    interaction_sessions = data[0]['interactionSessions']
        except Exception as e:
            print(f"Error reading interaction data: {e}")
            return

    # Initialize renderer with interaction data
    if interaction_sessions:
        # Save animation to video file
        for i, interaction_session in enumerate(interaction_sessions):
            renderer = GridRenderer(interpreter, interaction_session)
            renderer.save_animation(f"animation_{i}.mp4")
        print("Animation saved to animation.mp4")
    else:
        # Run interactive mode
        renderer.render()
        
        # Start command loop
        while True:
            try:
                inp = input(
                    "Enter command (click x y, step, left, right, up, down, q to quit): "
                )
            except EOFError:
                break

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
                    renderer.update_action_text(f"Click at ({x}, {y})")
                except ValueError:
                    print("Invalid coordinates. x and y should be integers.")
                    continue
            elif inp.lower() == "step":
                interpreter.step()
                print("Executed step.")
                renderer.update_action_text("Step")
            elif inp.lower() == "left":
                interpreter.left()
                print("Pushed Left.")
                renderer.update_action_text("Left")
            elif inp.lower() == "right":
                interpreter.right()
                print("Pushed Right.")
                renderer.update_action_text("Right")
            elif inp.lower() == "up":
                interpreter.up()
                print("Pushed Up.")
                renderer.update_action_text("Up")
            elif inp.lower() == "down":
                interpreter.down()
                print("Pushed Down.")
                renderer.update_action_text("Down")
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
