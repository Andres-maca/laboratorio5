# Practica 5 - Simulacion grafica de colisiones

## Que incluye

- Simulacion grafica en tiempo discreto dentro de una caja rectangular.
- 4 particulas iniciales con posicion, velocidad, masa y radio.
- 4 obstaculos cuadrados estaticos.
- Colisiones perfectamente elasticas contra paredes.
- Colisiones inelasticas contra obstaculos usando coeficiente de restitucion.
- Colisiones completamente inelasticas entre particulas: se unen y conservan el momento lineal.
- Ventana con trayectorias, particulas, obstaculos y registro de colisiones.
- Diagrama de clases: `DIAGRAMA_CLASES.md`.

## Archivos principales

- `main.cpp`: configura el escenario, crea la ventana Qt y actualiza la simulacion con `QTimer`.
- `simulador_colisiones.h/.cpp`: motor fisico reutilizable para consola o interfaz grafica.
- `particula_simulada.h/.cpp`: modelo de particula para multiples colisiones.
- `obstaculo.h/.cpp`: modelo de obstaculo, con restitucion y resistencia futura para el juego.
- `vector2d.h/.cpp`: operaciones vectoriales.
- `evento_colision.h/.cpp`: registro de colisiones.

## Como usarlo luego en el juego

La clase `CollisionSimulator` sigue separada de la ventana. La GUI actual llama `step()` desde un `QTimer`
y despues dibuja las particulas con `QGraphicsEllipseItem`. Para el juego por turnos, esta misma base se
puede extender agregando jugadores, proyectiles, turnos, dano y condiciones de victoria.

Ejemplo de integracion:

```cpp
QObject::connect(&timer, &QTimer::timeout, [&]() {
    simulator.step();
    const QList<Particle> particles = simulator.particles();
    // actualizar aqui los QGraphicsEllipseItem segun particle.position()
});
```

`Obstacle` ya tiene `resistance`, de modo que se puede extender la colision con obstaculos para restar:

```text
danio = factor_constante * masa_proyectil * velocidad_impacto
```

## Rama

El trabajo quedo en la rama local `simulacion-colisiones`.
