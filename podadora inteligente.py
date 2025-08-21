import pygame
import sys

pygame.init()

ANCHO, ALTO = 600, 400
VENTANA = pygame.display.set_mode((ANCHO, ALTO))
pygame.display.set_caption("Podadora con obstáculos (colisión precisa)")

# Colores
MARRON = (139, 69, 19)   # podadora
CESPED = (34, 139, 34)   # césped
TIERRA = (139, 115, 85)  # tierra
NEGRO = (0, 0, 0)        # obstáculos
BLANCO = (255, 255, 255)

class Podadora:
    def __init__(self, x, y, ancho=50, alto=30, velocidad=5):
        self.rect = pygame.Rect(x, y, ancho, alto)
        self.velocidad = velocidad

    def mover(self, teclas, obstaculos):
        dx = (teclas[pygame.K_RIGHT] - teclas[pygame.K_LEFT]) * self.velocidad
        dy = (teclas[pygame.K_DOWN] - teclas[pygame.K_UP]) * self.velocidad

        # --- mover en X y resolver colisiones ---
        if dx != 0:
            self.rect.x += dx
            # límites pantalla
            if self.rect.left < 0: self.rect.left = 0
            if self.rect.right > ANCHO: self.rect.right = ANCHO
            # colisiones con obstáculos
            for obs in obstaculos:
                if self.rect.colliderect(obs):
                    if dx > 0:
                        self.rect.right = obs.left  # pega al borde izquierdo del obstáculo
                    else:
                        self.rect.left = obs.right   # pega al borde derecho del obstáculo

        # --- mover en Y y resolver colisiones ---
        if dy != 0:
            self.rect.y += dy
            # límites pantalla
            if self.rect.top < 0: self.rect.top = 0
            if self.rect.bottom > ALTO: self.rect.bottom = ALTO
            # colisiones con obstáculos
            for obs in obstaculos:
                if self.rect.colliderect(obs):
                    if dy > 0:
                        self.rect.bottom = obs.top  # pega al borde superior del obstáculo
                    else:
                        self.rect.top = obs.bottom  # pega al borde inferior del obstáculo

    def dibujar(self, superficie):
        pygame.draw.rect(superficie, MARRON, self.rect)

# Fondo césped
fondo = pygame.Surface((ANCHO, ALTO))
fondo.fill(CESPED)

podadora = Podadora(ANCHO//2, ALTO//2)
obstaculos = []
modo_edicion = True

clock = pygame.time.Clock()
fuente = pygame.font.SysFont(None, 26)

while True:
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

        if modo_edicion and e.type == pygame.MOUSEBUTTONDOWN:
            x, y = e.pos
            # obstáculo 30x30 centrado en el click (hitbox = dibujo)
            obs = pygame.Rect(x - 15, y - 15, 30, 30)
            obstaculos.append(obs)

        if e.type == pygame.KEYDOWN and e.key == pygame.K_SPACE:
            modo_edicion = False  # empezar juego

    teclas = pygame.key.get_pressed()

    if not modo_edicion:
        podadora.mover(teclas, obstaculos)
        # rastro tierra (no pinta sobre obstáculos visibles porque se dibujan encima)
        pygame.draw.rect(fondo, TIERRA, podadora.rect)

    # Dibujar
    VENTANA.blit(fondo, (0, 0))
    for obs in obstaculos:
        pygame.draw.rect(VENTANA, NEGRO, obs)
    podadora.dibujar(VENTANA)

    if modo_edicion:
        texto = fuente.render("Modo edición: clic para obstáculos (negro). ESPACIO para iniciar.", True, BLANCO)
        VENTANA.blit(texto, (15, 10))

    pygame.display.flip()
    clock.tick(60)
