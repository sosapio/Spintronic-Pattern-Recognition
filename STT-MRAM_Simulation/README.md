The codes and README are origianlly built from Fernando Garc´ıa-Redondo et al. A Compact Model for
Scalable MTJ Simulation. arXiv:2106.04976 [cs]. June
2021. DOI: 10.48550/arXiv.2106.04976

The codes are adjusted by Minyeok Wi

## Files organization
* `README.md` for the MRAM python s-LLGS description
* `sllgs_solver.py` Python s-LLGS solver and exploration of multiple states MTJ      
* `plot.ipynb` s-LLGS simulation results, calling (`sllgs_solver.py`)
* `stochastic_multithread_simulation.py` (calling `sllgs_solver.py`) Multi-thread python stochastic simulations

## s-LLGS Solvers

### No-thermal and emulated-thermal simulations
* Use Scipy solver in python (`scipy_ivp=True`)
* Use Spherical coordinates
* Control the simulation through tolerances (`atol, rtol` in python)

```
    # No thermal, EMULATED THERMAL, solved with scipy_ivp
    llg_b = sllg_solver.LLG(do_fake_thermal=True,
                            d_theta_fake_th=1/30,
                            do_thermal=False,
                            i_amp_fn=current,
                            shape_ani_demag_n = True,
                            do_a_exchange = True,
                            seed=seed_0)
    data_b = llg_b.solve_and_plot(100e-9,
                                  scipy_ivp=True,
                                  solve_spherical=True,
                                  solve_normalized=True,
                                  max_step=1e-11,
                                  rtol=1e-4,
                                  atol=1e-9)
```
### Thermal Stochastic Simulations
Require stochastic differential equation solvers
* Use SDE solvers in python (`scipy_ivp=False`)
* Use Cartesian coordinates
* Control the simulation through maximum time step (`max_step` in python)
```
    llg_c = sllg_solver.LLG(do_fake_thermal=False,
                            do_thermal=True,
                            i_amp_fn=current,
                            shape_ani_demag_n = True,
                            do_a_exchange = True,
                            seed=seed_0)
    data_c = llg_c.solve_and_plot(100e-9,
                                  scipy_ivp=False,
                                  solve_spherical=False,
                                  solve_normalized=True,
                                  max_step=1e-13,
                                  method='stratonovich_heun')
```
