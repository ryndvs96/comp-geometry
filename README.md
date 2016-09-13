# Computation Geometry
Programs to evaluate and display conic sections based on the manipulation of line and circle formula. Applicable to constructing curves for airplane wings and fuselages.

## Defining Conics
We can define a triangle comprised of lines *L1*, *L2*, and *L3*.
We can then define a point *S* that will lie on the curve of our conic.

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/L1L2L3S.jpg "")

With these, we can define the conic as follows:

```
(L1 * L2) - (t * L3^2) = 0
```

Where *t* equals `L1 * L2 / L3^2` evaluated at *S*.

## Defining Curves

We can generalize the above formula to allow for more than just 3 lines.
We can get the product from a set of lines. Even further, we can get the
product from a set of circles and lines.
We can now define our curve with the following equation:

```
P1 - (t * P2) = 0
```

Where *P1* and *P2* are products of two distinct sets of lines and
circles, and *t* is equal to `P1 / P2` evaluated at the a point on the curve *S*.

Example:

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/Curves1.jpg "")
