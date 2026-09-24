import sys
import numpy as np
import pyvista as pv

sys.path.append("../build")
from CGNS_App import Parser


CGNS_PATH = "/home/alex/Diploma/CGNS/example/xyz-L-cut4-z1-ordered-T4.cgns"

PRESSURE_FLOW = 4

SCALAR_NAME = "Temperature"


sim = Parser(CGNS_PATH)
sim.ReadSolution()
sim.ReadCoordinates()

Coordinates  = sim.GetCoordinates()
FlowSolution = sim.GetFlowSolution()

FlowSolMap = sim.GetFlowSolMap()


n_zones = len(Coordinates)
print(f"Зон: {n_zones}")

for zone in range(n_zones):
    for flow in range(len(FlowSolution[zone][0])):
        print(f"Zone: {zone}; Flow: {flow}; Name: {FlowSolMap[zone][0][flow]}")

zones = []
clim_min = +np.inf
clim_max = -np.inf

for zone in range(n_zones):
    coords   = np.asarray(Coordinates[zone], dtype=float)              # (ni, nj, nk, 3)
    pressure = np.asarray(FlowSolution[zone][0][PRESSURE_FLOW], dtype=float)  # (ni-1, nj-1, nk-1)

    ni, nj, nk, _ = coords.shape

    print(f"Zone {zone}: coords {coords.shape}, pressure {pressure.shape}")

    assert pressure.shape == (ni - 1, nj - 1, nk - 1), \
        f"Зона {zone}: pressure.shape {pressure.shape} != " \
        f"ожидаемого {(ni-1, nj-1, nk-1)}"

    g = pv.StructuredGrid()
    g.points = coords.reshape(-1, 3, order="F")
    g.dimensions = (ni, nj, nk)

    g.cell_data[SCALAR_NAME] = pressure.ravel(order="F")

    # print(f"  g: {g.n_points} points, {g.n_cells} cells")
    # print(f"  bounds: {g.bounds}")
    # print(f"  extent: "
    #       f"X={g.bounds[1]-g.bounds[0]:.4f}, "
    #       f"Y={g.bounds[3]-g.bounds[2]:.4f}, "
    #       f"Z={g.bounds[5]-g.bounds[4]:.4f}")

    zones.append(g)

    lo, hi = g.get_data_range(SCALAR_NAME)
    clim_min = min(clim_min, lo)
    clim_max = max(clim_max, hi)

print(f"Общий диапазон '{SCALAR_NAME}': [{clim_min:.4f}, {clim_max:.4f}]")


pl = pv.Plotter(window_size=[1400, 900])

for g in zones:
    pl.add_mesh(
        g,
        scalars=SCALAR_NAME,
        cmap="jet",
        clim=(clim_min, clim_max),
        show_edges=False,
        opacity=0.9,
        show_scalar_bar=False,
    )


pl.add_scalar_bar(
    title=SCALAR_NAME, n_labels=5, fmt="%.3f",
    vertical=True, title_font_size=14, label_font_size=12,
    width=0.05, height=0.6,
    position_x=0.85, position_y=0.2,
)

pl.add_axes()
pl.add_text(f"{n_zones} zone(s), scalar = {SCALAR_NAME}", font_size=10)
pl.show()