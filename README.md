# Circuit Solver
![ezgif-1a5a67013d2db1](https://github.com/user-attachments/assets/14d22e43-93e7-4d4f-84bd-bc0734a25e8f)

## About
Circuit Solver written in cpp and raylib will use Kirchhoff's circuit laws to solve for voltages across resistors

## How to run
Only works on linux/unix
install [raylib](https://www.raylib.com)
then 
```
git clone https://github.com/manthanabc/CircuitSolver cs
cd cs
make run
```
## How to use
just hover over an edge to select it, press (Case sensitive)
- `R`: Add a resistor to the selected edge.
- `W`: Add a wire to the selected edge.
- `num[0-9]`: Switch the highlighted path (replace `num` with a number key, e.g., `0`, `1`, etc.).

## On Algorithm
Makes use of [kirchhoffs law](https://en.wikipedia.org/wiki/Kirchhoff%27s_circuit_laws) to decompose circuit into loops (In DFS like way), then calculating current across each loop. then summing the currents for each indivisual component;
