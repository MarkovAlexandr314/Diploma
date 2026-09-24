import sys
import numpy as np
import matplotlib.pyplot as plt

sys.path.append("/home/alex/Diploma/CGNS/Parser/build")
from CGNS_App import Parser


Simulation = Parser("/home/alex/Diploma/CGNS/example/xyz-L-cut4-z1-ordered-T4.cgns")

Simulation.ReadSolution()
FlowSolution = Simulation.GetFlowSolution()

Simulation.ReadCoordinates()
Coordinates = Simulation.GetCoordinates()

ZoneNameMap = Simulation.GetZoneNameMap()

print(f'Coord Size: {len(Coordinates[0])}')

n_zones = len(Coordinates)
PRESSURE_INDEX = 1   # ← подберите под свой CGNS-файл

xs, ys, zs, ps = [], [], [], []

for zone in range(n_zones):
    coords   = np.asarray(Coordinates[zone], dtype=float)   # (ni, nj, nk, 3)
    solution = np.asarray(FlowSolution[zone][0], dtype=float)  # (ni-1, nj-1, nk-1, nvars)

    # Координаты вершин → усекаем до размеров ячеек
    coords_cell = coords[:-1, :-1, :-1, :]   # (ni-1, nj-1, nk-1, 3)

    # Проверка согласованности
    assert coords_cell.shape[:3] == solution.shape[:3], \
        f"Зона {zone}: coords_cell {coords_cell.shape} vs solution {solution.shape}"

    xs.append(coords_cell[..., 0].ravel())
    ys.append(coords_cell[..., 1].ravel())
    zs.append(coords_cell[..., 2].ravel())
    ps.append(solution[...].ravel())

x = np.concatenate(xs)
y = np.concatenate(ys)
z = np.concatenate(zs)
p = np.concatenate(ps)

print(f"Всего точек: {x.size}")
assert x.size == y.size == z.size == p.size

# Прореживание для скорости отрисовки
step = max(1, x.size // 30000)

fig = plt.figure(figsize=(11, 9))
ax = fig.add_subplot(111, projection="3d")

sc = ax.scatter(
    x[::step], y[::step], z[::step],
    c=p[::step], cmap="jet", s=2, alpha=0.75,
    vmin=np.percentile(p, 2),
    vmax=np.percentile(p, 98),
)

cbar = fig.colorbar(sc, ax=ax, label="Pressure", shrink=0.6, pad=0.1)
ax.set_xlabel("X"); ax.set_ylabel("Y"); ax.set_zlabel("Z")
ax.set_title(f"Flow solution ({n_zones} zone(s), {x.size} cells)")
plt.tight_layout()
plt.show()