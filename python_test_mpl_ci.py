import interpreter_module
import json
import sys

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


if __name__ == "__main__":
    main()
