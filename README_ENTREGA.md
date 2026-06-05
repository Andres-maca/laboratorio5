# Practica 5 - Modulo 2

## Que incluye

- Juego grafico por turnos con dos jugadores.
- Cada jugador lanza un proyectil configurando angulo y velocidad inicial.
- Colisiones contra los limites del escenario con rebote y perdida progresiva de energia.
- Colisiones inelasticas contra infraestructura con coeficiente de restitucion.
- Infraestructura rectangular con resistencia numerica.
- Dano proporcional al momento lineal del proyectil:

```text
dano = factor_constante * masa_proyectil * velocidad_impacto
```

- Logica de turnos, dano y victoria.
- El representante rival solo puede ser vulnerado cuando toda su infraestructura esta destruida.
- Integracion grafica con Qt Widgets y Graphics View.
- Sprites integrados con `resources.qrc`: Totoro para los jugadores, slime animado para la base protegida y bloque para la infraestructura.
- Diagrama de clases en `DIAGRAMA_CLASES.md`.

## Archivos principales

- `main.cpp`: ventana principal, controles, dibujo del juego y temporizador.
- `juego_turnos.h/.cpp`: reglas del juego, turnos, disparos, dano, colisiones y victoria.
- `jugador.h/.cpp`: datos de cada jugador, posicion de disparo y representante.
- `proyectil.h/.cpp`: estado fisico del proyectil; reutiliza `Particle` por composicion.
- `infraestructura.h/.cpp`: resistencia, dueno, rectangulo y restitucion; reutiliza `Obstacle` por composicion.
- `particula_simulada.h/.cpp`: clase de particula del modulo 1 usada internamente por `Proyectil`.
- `obstaculo.h/.cpp`: clase de obstaculo del modulo 1 usada internamente por `Infraestructura`.
- `vector2d.h/.cpp`: operaciones vectoriales usadas en fisica.
- `evento_colision.h/.cpp`: registro de eventos del juego.
- `lab5.pro`: configuracion Qt con `core`, `gui` y `widgets`.
- `resources.qrc`: empaqueta las imagenes de la carpeta `srites` como recursos Qt.

## Rubrica cubierta

- Colisiones con limites: el proyectil invierte la componente normal y reduce su velocidad por restitucion.
- Colisiones inelasticas con infraestructura: se aplica restitucion y perdida de energia.
- Logica de juego: alternancia de turnos, calculo de dano y condicion de victoria.
- Diagrama de clases: incluido en `DIAGRAMA_CLASES.md`.
- Integracion grafica: escena, vista, temporizador, controles y panel de eventos usando Qt.
