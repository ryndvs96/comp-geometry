# Computational Geometry
This code and research was produced with Professor Cristoph Hoffmann, *Purdue University*. 
This program to evaluates and displays conic sections based on the manipulation of line and circle formula. Applicable to constructing curves for airplane wings and fuselages.

## Defining Conics
We can define a triangle comprised of lines *L1*, *L2*, and *L3*.
We can then define a point *S* that will lie on the curve of our conic.

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/L1L2L3S.jpg)

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

In the example below, the set of circles and lines in purple contribute to *P1* and the set in yellow contribute to *P2*.

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/Curves1.jpg)

## Applications

Below is an example of how one might represent a 2D slice of an aircraft wing with just one curve equation as opposed to multiple conic equations.

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/Wing.jpg)

## Implementation

One major problem when producing the curve is that it isn’t guaranteed to be a function. This means that
you can’t simply evaluate an x coordinate and plot the corresponding y coordinate, because there could be
multiple y values that the curve evaluates to for a given x coordinate. The simplest way to get around
this is to evaluate the curve for every x-y coordinate pair on the grid, and only plot the curve where the
coordinates evaluate to zero. For this, we must define a density of our grid because we cannot iterate over
an infinite amount of x-y coordinates. 

The example below shows a density grid and the points at which the products were evaluated.

![Imagine there's a picture here displaying what I've just described...](https://raw.githubusercontent.com/ryndvs96/comp-geometry/master/examples/Curves2.jpg)
