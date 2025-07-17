using AutumnInterpreter: Interpreter, run_script, step, click, left, right, up, down,
                        render_all
using JSON, CxxWrap, PlotlyJS, WebIO, Observables, JSExpr

# ------------------------------------------------------------------
const CELL_SIZE = 1.0
const MARGIN    = 0.05
const COLOR_MAP = Dict(
    "gray"=>"gray","gold"=>"gold","green"=>"green","mediumpurple"=>"mediumpurple",
    "purple"=>"purple","white"=>"white","yellow"=>"yellow","blue"=>"blue",
    "red"=>"red","orange"=>"orange")

parse_grid(out::CxxWrap.StdLib.StdStringAllocated) = let d=JSON.parse(String(out)); s=pop!(d,"GRID_SIZE",0); (d,s) end
parse_grid(out::AbstractString) = let d=JSON.parse(out); s=pop!(d,"GRID_SIZE",0); (d,s) end

rectangle_trace(x0,y0;color="black",outline=true) = scatter(
    x=[x0,x0+CELL_SIZE,x0+CELL_SIZE,x0,x0],
    y=[y0,y0,y0+CELL_SIZE,y0+CELL_SIZE,y0],
    mode="lines",fill="toself",fillcolor=color,
    line=attr(color=outline ? "white" : color,width=1),hoverinfo="none")

mutable struct GridRenderer
    interpreter::Interpreter
    grid::Dict{String,Any}
    gridsize::Int
    plot::Union{PlotlyJS.SyncPlot,Nothing}
    masking::Bool
    masked::Set{Tuple{Int,Int}}
    frame::Int
    history::Vector{Dict}
    running::Bool
    lasttick::Float64
    function GridRenderer(interp)
        new(interp,Dict(),0,nothing,false,Set{Tuple{Int,Int}}(),0,Dict[],true,time())
    end
end

build_traces(grid,n) = begin
    tr = GenericTrace[]
    for objs in values(grid), obj in objs
        col=obj["position"]["x"]; row=obj["position"]["y"]
        key=lowercase(obj["color"])
        x0 = MARGIN + col*(CELL_SIZE+MARGIN)
        y0 = MARGIN + (n-1-row)*(CELL_SIZE+MARGIN)
        push!(tr, rectangle_trace(x0,y0;color=get(COLOR_MAP,key,key)))
    end
    tr
end

function render!(renderer::GridRenderer)
    render_output = render_all(renderer.interpreter)
    grid, gridsize = parse_grid(render_output)
    renderer.grid = grid
    renderer.gridsize = gridsize

    # Build traces using the existing build_traces function
    traces = build_traces(grid, gridsize)
    
    # Add frame counter trace
    push!(traces, scatter(
        x=[0.02 * gridsize],
        y=[0.98 * gridsize],
        text=["Frame: $(renderer.frame)"],
        mode="text",
        textfont=attr(color="white", size=10),
        hoverinfo="none"
    ))

    # Add masked cells traces
    for (col, row) in renderer.masked
        x0 = MARGIN + col*(CELL_SIZE+MARGIN)
        y0 = MARGIN + (gridsize-1-row)*(CELL_SIZE+MARGIN)
        push!(traces, rectangle_trace(x0, y0, color="gray", outline=true))
    end

    layout = Layout(
        width=500,
        height=500,
        plot_bgcolor="black",
        paper_bgcolor="black",
        showlegend=false,
        xaxis=attr(
            range=[-2, gridsize + 2],
            showgrid=false,
            zeroline=false,
            showline=false,
            showticklabels=false
        ),
        yaxis=attr(
            range=[-2, gridsize + 2],
            showgrid=false,
            zeroline=false,
            showline=false,
            showticklabels=false,
            scaleanchor="x"
        ),
        margin=attr(l=0, r=0, t=0, b=0)
    )

    if isnothing(renderer.plot)
        # First time rendering - create new plot
        plt = PlotlyJS.plot(traces, layout)
        
        # Set up event listeners only once
        onimport(plt.scope, @js function(node)
            window.addEventListener("keydown", (e) -> WebIO.dispatch(node, "keydown", e.key))
        end)
        
        display(plt)
        renderer.plot = plt
    else
        # Update existing plot
        PlotlyJS.react!(renderer.plot, traces, layout)
    end
end

function handle_click!(r::GridRenderer, x, y)
    col = Int(floor((x-MARGIN)/(CELL_SIZE+MARGIN)))
    row = Int(floor((r.gridsize - y - MARGIN)/(CELL_SIZE+MARGIN)))
    (0<=col<r.gridsize && 0<=row<r.gridsize) || return
    
    if r.masking
        coord = (col,row)
        if in(coord, r.masked)
            delete!(r.masked, coord)
        else
            push!(r.masked, coord)
        end
    else
        click(r.interpreter, col, row)
        step(r.interpreter)
    end
    store_frame!(r)
    render!(r)
end

function handle_key!(r::GridRenderer, key::String)
    if key == "ArrowUp"
        up(r.interpreter)
    elseif key == "ArrowDown"
        down(r.interpreter)
    elseif key == "ArrowRight"
        right(r.interpreter)
    elseif key == "ArrowLeft"
        left(r.interpreter)
    elseif key == "m"
        r.masking = !r.masking
        println("Masking mode: ", r.masking ? "enabled" : "disabled")
    elseif key == "c"
        empty!(r.masked)
    end
    
    if key in ["ArrowUp", "ArrowDown", "ArrowRight", "ArrowLeft"]
        step(r.interpreter)
        store_frame!(r)
    end
    render!(r)
end

store_frame!(r::GridRenderer) = (push!(r.history,Dict("grid"=>deepcopy(r.grid),"masked"=>collect(r.masked))); r.frame+=1)

autorefresh(r::GridRenderer;dt=0.3) = while true
    if r.running && (time()-r.lasttick)>=dt
        step(r.interpreter); store_frame!(r); render!(r); r.lasttick=time()
    end
    sleep(0.01)
end

function main()
    interp = Interpreter()
    script = length(ARGS)>0 ? ARGS[1] : "./tests/ants.sexp"
    run_script(interp, read(script,String), "", "")
    r = GridRenderer(interp)
    render!(r)
    println("Controls: click | arrows | m mask | c clear | p pause | s save frames")
    autorefresh(r)
end

if abspath(PROGRAM_FILE)==@__FILE__
    main()
end
