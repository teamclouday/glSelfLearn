## Some Math operations needed in OpenGL  

------

### Vector  
**Dot Product**  
$$V1\times{V2}=V1.x\times{V2.x}+V1.y\times{V2.y}+V1.z\times{V2.z}$$  

**Cross Product**  
$$V1\times{V2}=\|V1\|\|V2\|\sin(\theta)n$$  
where n is the unit normal to the V1 V2 plane  

cross product of 3d vector:  
$$\begin{bmatrix}V3.x\\V3.y\\V3.z\end{bmatrix}=\begin{bmatrix}V1.y\cdot{V2.z}-V1.z\cdot{V2.y}\\V1.z\cdot{V2.x}-V1.x\cdot{V2.z}\\V1.x\cdot{V2.y}-V1.y\cdot{V2.x}\end{bmatrix}$$  

**Vector Length**  
$$\text{length}(v)=\sqrt{v.x^2+v.y^2+v.z^2}$$  

**Reflection**  
$$R_\text{reflect}=R_\text{in}-(2N\cdot{R_\text{in}})N$$  

**Refraction**  
Given $\eta$  
$$k=1-\eta^2(1-(N\cdot{R})^2)$$  
$$R_\text{reflect}=\begin{cases}0.0 & \text{if}\:k<0.0\\\eta{R}-(\eta(N\cdot{R})+\sqrt{k})N & \text{if}\:k\geq0.0\end{cases}$$  
$R$ and $N$ should both be unit length  

------

### Matrices  
**Construction**  
```cpp
GLfloat matrix[4][4];
```  
$$\begin{bmatrix}A_{00} & A_{10} & A_{20} & A_{30}\\A_{01} & A_{11} & A_{21} & A_{31}\\A_{02} & A_{12} & A_{22} & A_{31}\\A_{03} & A_{13} & A_{23} & A_{33}\end{bmatrix}$$  

column-major order:  
```cpp
static const float A[] =
{
    A00, A01, A02, A03, A10, A11, A12, A13,
    A20, A21, A22, A23, A30, A31, A32, A33
};
```  
row-major order:  
```cpp
static const float A[] =
{
    A00, A10, A20, A31, A01, A11, A21, A31,
    A02, A12, A22, A32, A03, A13, A23, A33
};
```  

**Rotation and Translation**  
$$\begin{bmatrix}\alpha_{0,0}&\alpha_{1,0}&\alpha_{2,0}&\beta_0\\\alpha_{0,1}&\alpha_{1,1}&\alpha_{2,1}&\beta_1\\\alpha_{0,2}&\alpha_{1,2}&\alpha_{2,2}&\beta_2\\0.0&0.0&0.0&1.0\end{bmatrix}$$  
$\alpha$ part represents rotation or orientation, $\beta$ part represents translation or position  

------

### Coordinate Spaces  

Coordinate Space | What it represents  
--- | ---  
Model Space | Positions relative to local origin  
World Space | Positions relative to global origin  
View Space | Positions relative to the viewer (camera)  
Clip Space | Positions of vertices after projection into nonlinear homogenous coordinates  
Normalized Device Coordinate (NDC) Space | Vertex Coordinate after divided by their own $w$ component  
Window Space | Positions of vertices in pixels, relative to origin of the window  

**Identity Matrix**  
$$\begin{bmatrix}1.0&0.0&0.0&0.0\\0.0&1.0&0.0&0.0\\0.0&0.0&1.0&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

**Translation Matrix**  
$$\begin{bmatrix}1.0&0.0&0.0&t_x\\0.0&1.0&0.0&t_y\\0.0&0.0&1.0&t_z\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

**Rotation Matrix**  
rotate about $x$ axis:  
$$R_x(\theta)=\begin{bmatrix}1.0&0.0&0.0&0.0\\0.0&\cos(\theta)&\sin(\theta)&0.0\\0.0&-\sin(\theta)&\cos(\theta)&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  
rotate about $y$ axis:  
$$R_y(\theta)=\begin{bmatrix}\cos(\theta)&0.0&-\sin(\theta)&0.0\\0.0&1.0&0.0&0.0\\\sin(\theta)&0.0&\cos(\theta)&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  
rotate about $z$ axis:  
$$R_z(\theta)=\begin{bmatrix}\cos(\theta)&-\sin(\theta)&0.0&0.0\\\sin(\theta)&\cos(\theta)&0.0&0.0\\0.0&0.0&1.0&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

multiply together:  
$$R_z(\psi)R_y(\theta)R_x(\phi)=\begin{bmatrix}\cos(\theta)\cos(\psi)&\cos(\phi)\sin(\psi)+\sin(\phi)\sin(\theta)\cos(\psi)&\sin(\phi)\sin(\psi)-\cos(\phi)\sin(\theta)\cos(\psi)&0.0\\-\cos(\theta)\sin(\psi)&\cos(\phi)\cos(\psi)-\sin(\phi)\sin(\theta)\sin(\psi)&\sin(\phi)\cos(\psi)+\cos(\phi)\sin(\theta)\sin(\psi)&0.0\\\sin(\theta)&-\sin(\phi)\cos(\theta)&\cos(\phi)\cos(\theta)&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

Eular's Angles -- problem: gimbal lock  
Better to use quaternions instead  

**Scaling Matrix**  
$$\begin{bmatrix}s_x&0.0&0.0&0.0\\0.0&s_y&0.0&0.0\\0.0&0.0&s_z&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

**Quaternions**  
One real part and three imaginary parts  
$$q=(x+yi+zj+wk)$$  
$$i^2=k^2=k^2=ijk=-1$$  
$$i=jk\;\text{and}\;j=ik\;\text{and}\;k=ij$$  

------
**Lookat Matrix**  
Construct a rotation matrix:  
$e$ is the camera position, $p$ is the point to transform, $u$ is the up vector  
First, construct forward vector $f$:  
$$f=\frac{p-e}{\|p-e\|}$$  
Next, take cross product of $f$ and $u$ to construct a side vector $s$:  
$$s=f\times{u}$$  
Then construct a new up vector in camera's reference:  
$$u'=s\times{f}$$  
Finally, construct a rotation matrix:  
$$R=\begin{bmatrix}s.x&u'.x&f.x&0.0\\s.y&u'.y&f.y&0.0\\s.z&u'.z&f.z&0.0\\0.0&0.0&0.0&1.0\end{bmatrix}$$  
Also need to transform the origin to the position of the camera:  
$$T=\begin{bmatrix}s.x&u'.x&f.x&-e.x\\s.y&u'.y&f.y&-e.y\\s.z&u'.z&f.z&-e.z\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

**Perspective Matrices**  
Given near and far, left, right, top and bottom  
$$\begin{bmatrix}\frac{2\cdot\text{near}}{\text{right}-\text{left}}&0.0&\frac{\text{right}+\text{left}}{\text{right}-\text{left}}&0.0\\0.0&\frac{2\cdot\text{near}}{\text{top}-\text{bottom}}&\frac{\text{top}+\text{bottom}}{\text{top}-\text{bottom}}&0.0\\0.0&0.0&\frac{\text{near}+\text{far}}{\text{near}-\text{far}}&\frac{2\cdot\text{near}\cdot\text{far}}{\text{near}-\text{far}}\\0.0&0.0&-1.0&0.0\end{bmatrix}$$  

**Orthographic Matrices**  
Map view-space coordinates to clip-space coordinates (less real than perspective projection)  
$$\begin{bmatrix}\frac{2}{\text{right}-\text{left}}&0.0&0.0&\frac{\text{left}+\text{right}}{\text{left}-\text{right}}\\0.0&\frac{2}{\text{top}-\text{bottom}}&0.0&\frac{\text{bottom}+\text{top}}{\text{bottom}-\text{top}}\\0.0&0.0&\frac{2}{\text{near}-\text{far}}&\frac{\text{near}+\text{far}}{\text{far}-\text{near}}\\0.0&0.0&0.0&1.0\end{bmatrix}$$  

**Curves**  
1. Three control points A, B and C, with P on the curve  
   $D=A+t(B-A)$  
   $E=B+t(C-B)$  
   $P=D+t(E-D)$  
   $P=A+t(B-A)+t((B+t(C-B))-(A+t(B-A)))$  
   $P=A+2t(B-A)+t^2(C-2B+A)$  
   (which is called *quadratic Bezier curve*)  
2. Four control points A, B, C and D, with P on the curve  
   $E=A+t(B-A)$  
   $F=B+t(C-B)$  
   $G=C+t(D-C)$  
   $H=E+t(F-E)$  
   $I=F+t(G-F)$  
   $P=H+t(I-H)$  
   (which is called *cubic Bezier curve*)  

**Splines**  
Made up of several smaller curves  