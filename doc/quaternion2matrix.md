# Calculation from Quaternion to Rotation Matrix

## Formula

A rotation matrix can be derived from a unit quaternion that represents a 3D rotation.  
A quaternion is typically of the form:

q = w + xi + yj + zk

Where:

- w is the scalar part,
- x, y, z are the vector parts (imaginary components).

Given a quaternion q = w + xi + yj + zk, the corresponding 3x3 rotation matrix can be computed using the following formula:

Rotation matrix (3x3) element formulas:

| Column 1         | Column 2         | Column 3         |
|------------------|------------------|------------------|
| 1 - 2(y^2 + z^2) | 2(xy - zw)       | 2(xz + yw)       |
| 2(xy + zw)       | 1 - 2(x^2 + z^2) | 2(yz - xw)       |
| 2(xz - yw)       | 2(yz + xw)       | 1 - 2(x^2 + y^2) |

Where:

- w, x, y, z are the components of the quaternion q,
- (x, y, z) are the vector part of the quaternion,
- w is the scalar part of the quaternion.

## Steps to Calculate the Rotation Matrix:

**Given quaternion**: Let q = w + xi + yj + zk.

**Compute the individual terms**:

- Compute x^2, y^2, z^2.
- Compute 2xy, 2xz, 2yz.

**Fill in the rotation matrix** using the formula.

## Example

Let q = 1 + 2i + 3j + 4k.

### Step 1: Extract components

- w = 1
- x = 2
- y = 3
- z = 4

### Step 2: Compute necessary terms

- x^2 = 2^2 = 4
- y^2 = 3^2 = 9
- z^2 = 4^2 = 16
- 2xy = 2 * 2 * 3 = 12
- 2xz = 2 * 2 * 4 = 16
- 2yz = 2 * 3 * 4 = 24
- 2(xy + zw) = 2 * (2*3 + 1*4) = 2 * (6 + 4) = 20
- 2(xz - yw) = 2 * (2*4 - 3*1) = 2 * (8 - 3) = 10
- 2(yz + xw) = 2 * (3*4 + 2*1) = 2 * (12 + 2) = 28

### Step 3: Construct the matrix

Rotation matrix 3x3:

| Column 1      | Column 2      | Column 3     |
|---------------|---------------|--------------|
| 1 - 2(9 + 16) | 2(12 - 4)     | 2(16 + 3)    |
| 2(12 + 4)     | 1 - 2(4 + 16) | 2(24 - 2)    |
| 2(16 - 3)     | 2(24 + 2)     | 1 - 2(4 + 9) |

Thus, the resulting 3x3 rotation matrix R is:

| Column 1 | Column 2 | Column 3 |
|----------|----------|----------|
| -49      | 16       | 38       |
| 32       | -26      | 44       |
| 26       | 52       | -22      |


## Mathlab for Testing and Conformation

### Quaternion to Matrix

This Mathlab script is used to check rotation from a matrix retrieved from a quaternion.  

```javascript
% 'Define the rotation matrix R from quaternion (1, 2, 3, 4) 
R0 = [
 -0.6666666666667,  0.13333333333, 0.73333333333;
  0.6666666666667, -0.33333333333, 0.66666666667;
  0.3333333333333,  0.93333333333, 0.13333333333;
];

% 'Rotation matrix using Mathlab function. 
R = quat2rotm(quaternion(1, 2, 3, 4))

% Define the vector v
x = 1; y = 2; z = 3;
v = [x; y; z];

x1 = x * R(1,1) + y * R(1,2) + z * R(1,3)
y1 = x * R(2,1) + y * R(2,2) + z * R(2,3)
z1 = x * R(3,1) + y * R(3,2) + z * R(3,3)

% 'Apply the rotation matrix to the vector
v_rotated = R * v;

% 'Display the rotated vector
disp('The rotated vector is:');
disp(v_rotated);
```

### Quaternion Logarithmic Interpolation 

```
% Quaternion interpolation using logarithmic and exponential functions in MATLAB.

% Define the initial quaternions q1 and q2.
q1 = [1.0, 2.0, 3.0, 4.0];
q2 = [4.0, 3.0, 2.0, 1.0];

% Set interpolation factor
t = 0.5;

% Normalize the quaternions (important for accurate results).
q1 = q1 / norm(q1);
q2 = q2 / norm(q2);

% Compute the inverse of q1
q1_inv = quatconj(q1);

% Calculate the difference quaternion q_diff = q2 * q1_inv
q_diff = quatmultiply(q2, q1_inv);

% Take the logarithm of q_diff (only for the vector part)
log_q_diff = quatlog(q_diff);

% Scale the log by the interpolation factor t.
log_q_scaled = t * log_q_diff;

% Take the exponential to return to quaternion space
exp_q_interp = quatexp(log_q_scaled);

% Multiply by q1 to get the interpolated quaternion
q_result = quatmultiply(exp_q_interp, q1);

% Display the result
disp('Interpolated Quaternion (t=0.5):');
```

### Quaternion from Matrix

```
% Create a normalize quaternion.
q1 = [1, 2, 3, 4];
q1 = q1 / norm(q1);
% Create the rotation matrix from the quaternion.
R = quat2rotm(q);

% Reverse check.
assert(~isequal(R, q1), 'This must be the same.')

% Ensure R is a valid rotation matrix by checking the determinant.
if abs(det(R) - 1) > 1e-6
    error('Determinant of matrix is not equal to zero!');
end

% Ensure R is a valid rotation matrix by checking the matrix inverse
% multiplication is an identity matrix.
if ~isequal(ismembertol(R* R', [1, 1, 1,;1, 1, 1;1, 1, 1], 1e-12), eye(3.0))
    error('The matrix multiplied by its inverse self is not an identity matrix!');
end

% Calculate the trace of the matrix
trace_R = trace(R);

if trace_R > 0
    S = 2 * sqrt(trace_R + 1);
    w = 0.25 * S;
    x = (R(3, 2) - R(2, 3)) / S;
    y = (R(1, 3) - R(3, 1)) / S;
    z = (R(2, 1) - R(1, 2)) / S;
elseif R(1, 1) > R(2, 2) && R(1, 1) > R(3, 3)
    S = 2 * sqrt(1 + R(1, 1) - R(2, 2) - R(3, 3));
    w = (R(3, 2) - R(2, 3)) / S;
    x = 0.25 * S;
    y = (R(1, 2) + R(2, 1)) / S;
    z = (R(1, 3) + R(3, 1)) / S;
elseif R(2, 2) > R(3, 3)
    S = 2 * sqrt(1 + R(2, 2) - R(1, 1) - R(3, 3));
    w = (R(1, 3) - R(3, 1)) / S;
    x = (R(1, 2) + R(2, 1)) / S;
    y = 0.25 * S;
    z = (R(2, 3) + R(3, 2)) / S;
else
    S = 2 * sqrt(1 + R(3, 3) - R(1, 1) - R(2, 2));
    w = (R(2, 1) - R(1, 2)) / S;
    x = (R(1, 3) + R(3, 1)) / S;
    y = (R(2, 3) + R(3, 2)) / S;
    z = 0.25 * S;
end

% A quaternion as [w, x, y, z]
q2 = [w, x, y, z];

disp(q1)
disp(q2)

% Check for equality.
if ~isequal(q1, q2)
   disp('Success: Quaternions are the same.') 
else
   disp('Failure: Quaternions are NOT the same!') 
end
```

### Matrix3x3 Determinant

```
% Establish a 3x3 matrix.
A = [1 2 3; 4 5 6; 7 8 9];

% Compute determinant
detA = A(1,1)*(A(2,2)*A(3,3) - A(2,3)*A(3,2)) ...
     - A(1,2)*(A(2,1)*A(3,3) - A(2,3)*A(3,1)) ...
     + A(1,3)*(A(2,1)*A(3,2) - A(2,2)*A(3,1));

disp('Determinant of the matrix:');
disp(detA);
```