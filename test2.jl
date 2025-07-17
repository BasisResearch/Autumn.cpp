using AutumnInterpreter: Interpreter, run_script, step, click, left, right, up, down,
                        render_all
using JSON, CxxWrap, PlotlyJS, WebIO, Observables, JSExpr
using JSON3

const CELL_SIZE = 1.0
const MARGIN = 0.05
const COLOR_MAP = Dict(
    "gray"=>"gray","gold"=>"gold","green"=>"green","mediumpurple"=>"mediumpurple",
    "purple"=>"purple","white"=>"white","yellow"=>"yellow","blue"=>"blue",
    "red"=>"red","orange"=>"orange")

struct Position
    x::Int
    y::Int
end

struct Cell
    pos::Position
    color::String
    opacity::Float64
end

function rectangle_trace(x0,y0;color="black",outline=true)
    scatter(
        x=[x0,x0+CELL_SIZE,x0+CELL_SIZE,x0,x0],
        y=[y0,y0,y0+CELL_SIZE,y0+CELL_SIZE,y0],
        mode="lines",fill="toself",fillcolor=color,
        line=attr(color=outline ? "white" : color,width=1),hoverinfo="none"
    )
end

function build_traces(cells::Vector{Cell}, grid_size::Int)
    traces = GenericTrace[]
    for cell in cells
        x0 = MARGIN + cell.pos.x*(CELL_SIZE+MARGIN)
        y0 = MARGIN + (grid_size-1-cell.pos.y)*(CELL_SIZE+MARGIN)
        color = get(COLOR_MAP, lowercase(cell.color), cell.color)
        push!(traces, rectangle_trace(x0, y0, color=color))
    end
    traces
end

function render_frame(cells::Vector{Cell}, grid_size::Int)
    traces = build_traces(cells, grid_size)
    
    layout = Layout(
        width=500,
        height=500,
        plot_bgcolor="black",
        paper_bgcolor="black",
        showlegend=false,
        xaxis=attr(
            range=[-2, grid_size + 2],
            showgrid=false,
            zeroline=false,
            showline=false,
            showticklabels=false
        ),
        yaxis=attr(
            range=[-2, grid_size + 2],
            showgrid=false,
            zeroline=false,
            showline=false,
            showticklabels=false,
            scaleanchor="x"
        ),
        margin=attr(l=0, r=0, t=0, b=0)
    )

    plt = PlotlyJS.plot(traces, layout)
    # keep this plot open   
    display(plt)
    # wait for 10 seconds
    sleep(10)
    # Save the plot to a file
    savefig(plt, "plot.html")
    return plt
end

function get_cells_from_interpreter(interpreter::Interpreter)
    str = render_all(interpreter) |> String
    println(str)
    objects = JSON3.read(str)
    grid_size = objects[:GRID_SIZE]
    cells = Cell[]
    
    for (name, obj_array) in objects
        if name != :GRID_SIZE
            for cell_obj in obj_array
                if hasproperty(cell_obj, :position) && hasproperty(cell_obj.position, :x) && hasproperty(cell_obj.position, :y)
                    pos = Position(cell_obj.position.x, cell_obj.position.y)
                    cell = Cell(pos, cell_obj.color, get(cell_obj, :opacity, 1.0))
                    push!(cells, cell)
                end
            end
        end
    end
    
    return cells, grid_size
end

interpreter = Interpreter()
program = read(joinpath(@__DIR__, "tests/grow.sexp"), String)
run_script(interpreter, program, read(joinpath(@__DIR__, "autumnstdlib/stdlib.sexp"), String), "")
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
click(interpreter, 6, 8)
step(interpreter)
click(interpreter, 7, 8)
step(interpreter)
click(interpreter, 8, 8)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
step(interpreter)
goal_state, grid_size = get_cells_from_interpreter(interpreter)
println(goal_state)
println(grid_size)

render_frame(goal_state, grid_size)
