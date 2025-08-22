import pygame
import sys
from collections import deque

pygame.init()

ANCHO, ALTO = 600, 400
CELL = 20  # tamaño de la celda
VENTANA = pygame.display.set_mode((ANCHO, ALTO))
pygame.display.set_caption("Podadora en zig-zag (evita obstáculos)")

# Colores
MARRON = (139, 69, 19)   # podadora
CESPED = (34, 139, 34)   # césped
TIERRA = (139, 115, 85)  # tierra
NEGRO = (0, 0, 0)        # obstáculos

class Podadora:
    def __init__(self, x, y, size=20, velocidad=10):
        self.rect = pygame.Rect(x, y, size, size)
        self.path = deque()
        self.target = None
        self.velocidad = velocidad

    def dibujar(self, superficie):
        pygame.draw.rect(superficie, MARRON, self.rect)

    def mover(self):
        if self.target is None and self.path:
            self.target = self.path.popleft()

        if self.target:
            tx, ty = self.target

            # mover con “clamp” para no saltar el target
            dx = tx - self.rect.x
            if dx != 0:
                paso = self.velocidad if abs(dx) > self.velocidad else abs(dx)
                self.rect.x += paso if dx > 0 else -paso

            dy = ty - self.rect.y
            if dy != 0:
                paso = self.velocidad if abs(dy) > self.velocidad else abs(dy)
                self.rect.y += paso if dy > 0 else -paso

            if self.rect.topleft == (tx, ty):
                self.target = None

# Fondo césped
fondo = pygame.Surface((ANCHO, ALTO))
fondo.fill(CESPED)

# Obstáculos
obstaculos = [
    pygame.Rect(100, 100, 60, 60),
    pygame.Rect(300, 150, 80, 40),
    pygame.Rect(450, 250, 60, 60),
]

# Grilla
cols, rows = ANCHO // CELL, ALTO // CELL

# Mapa de bloqueos
bloqueado = [[False]*rows for _ in range(cols)]
for obs in obstaculos:
    x1, y1 = obs.left // CELL, obs.top // CELL
    x2, y2 = (obs.right-1)//CELL, (obs.bottom-1)//CELL
    for i in range(x1, x2+1):
        for j in range(y1, y2+1):
            bloqueado[i][j] = True

# Orden de cobertura: zig-zag por filas (solo celdas libres)
def generar_orden_zigzag():
    orden = []
    for j in range(rows):
        rango = range(cols) if j % 2 == 0 else range(cols-1, -1, -1)
        for i in rango:
            if not bloqueado[i][j]:
                orden.append((i, j))
    return orden

# BFS para camino más corto entre celdas libres (4 vecinos)
def bfs_camino(a, b):
    if a == b:
        return [a]
    ax, ay = a
    bx, by = b
    q = deque([(ax, ay)])
    prev = { (ax, ay): None }
    dirs = [(1,0), (-1,0), (0,1), (0,-1)]

    while q:
        x, y = q.popleft()
        for dx, dy in dirs:
            nx, ny = x + dx, y + dy
            if 0 <= nx < cols and 0 <= ny < rows and not bloqueado[nx][ny] and (nx, ny) not in prev:
                prev[(nx, ny)] = (x, y)
                if (nx, ny) == (bx, by):
                    # reconstruir
                    camino = [(bx, by)]
                    cur = (bx, by)
                    while cur is not None:
                        cur = prev[cur]
                        if cur is not None:
                            camino.append(cur)
                    camino.reverse()
                    return camino
                q.append((nx, ny))
    return None  # no alcanzable

# Construir ruta completa caminable uniendo cada objetivo con BFS
orden = generar_orden_zigzag()
if not orden:
    print("No hay celdas libres.")
    pygame.quit()
    sys.exit()

# Punto de inicio: primera celda libre del orden
start_cell = orden[0]

ruta_celdas = [start_cell]
actual = start_cell
for objetivo in orden[1:]:
    camino = bfs_camino(actual, objetivo)
    if camino:
        ruta_celdas.extend(camino[1:])  # evitar duplicar 'actual'
        actual = objetivo
    # si no hay camino, se omite ese objetivo (p.ej. aislado)

# Convertir celdas a pixeles (esquinas superiores izquierdas)
ruta_pixels = [(i*CELL, j*CELL) for (i, j) in ruta_celdas]

# Podadora inicial
podadora = Podadora(ruta_pixels[0][0], ruta_pixels[0][1], CELL)

# Cargar ruta
from collections import deque as dq
podadora.path = dq(ruta_pixels)

clock = pygame.time.Clock()

while True:
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    # Deja rastro de tierra donde pasa
    pygame.draw.rect(fondo, TIERRA, podadora.rect)

    # Mover siguiendo solo celdas vecinas (no atraviesa obstáculos)
    podadora.mover()

    # Dibujar
    VENTANA.blit(fondo, (0, 0))
    for obs in obstaculos:
        pygame.draw.rect(VENTANA, NEGRO, obs)
    podadora.dibujar(VENTANA)

    pygame.display.flip()
    clock.tick(60)
