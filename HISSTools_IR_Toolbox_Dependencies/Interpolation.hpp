
#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

// Enumeration of interpolation types

enum class InterpType { None, Linear, CubicHermite, CubicLagrange, CubicBSpline };

// Linear

template <class T>
struct linear_interp
{
    T operator()(const T& x, const T& y0, const T& y1) { return  (y0 + x * ((y1 - y0))); }
};

// Cubic Hermite

template <class T>
struct cubic_hermite_interp
{
    cubic_hermite_interp() : _5div2(2.5), _3div2(1.5), _1div2(0.5) {}
    
    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T c0 = y1;
        const T c1 = _1div2 * (y2 - y0);
        const T c2 = y0 - _5div2 * y1 + y2 + y2 - _1div2 * y3;
        const T c3 = _1div2 * (y3 - y0) + _3div2 * (y1 - y2);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
private:
    
    const T _5div2;
    const T _3div2;
    const T _1div2;
};

// Cubic Lagrange

template <class T>
struct cubic_lagrange_interp
{
    cubic_lagrange_interp() : _1div3(T(1)/T(3)), _1div6(T(1)/T(6)), _1div2(0.5) {}
    
    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T c0 = y1;
        const T c1 = y2 - _1div3 * y0 - _1div2 * y1 - _1div6 * y3;
        const T c2 = _1div2 * (y0 + y2) - y1;
        const T c3 = _1div6 * (y3 - y0) + _1div2 * (y1 - y2);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
private:

    const T _1div3;
    const T _1div6;
    const T _1div2;
};

// Cubic B-spline

template <class T>
struct cubic_bspline_interp
{
    cubic_bspline_interp() : _2div3(T(2)/T(3)), _1div6(T(1)/T(6)), _1div2(0.5) {}
    
    T operator()(const T& x, const T& y0, const T& y1, const T& y2, const T& y3)
    {
        const T y0py2 = y0 + y2;
        const T c0 = _1div6 * y0py2 + _2div3 * y1;
        const T c1 = _1div2 * (y2 - y0);
        const T c2 = _1div2 * y0py2 - y1;
        const T c3 = _1div2 * (y1 - y2) + _1div6 * (y3 - y0);
        
        return (((c3 * x + c2) * x + c1) * x + c0);
    }
    
private:

    const T _2div3;
    const T _1div6;
    const T _1div2;
};

#endif /* Interpolation_h */
