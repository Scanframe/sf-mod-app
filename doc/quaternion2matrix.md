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


```javascript
% Define the rotation matrix R
R0 = [
 -0.6666666667,     0.13333333333,   0.733333333;
  0.6666666666667, -0.3333333333333, 0.6666666666666667;
  0.3333333333333,  0.9333333333,    0.1333333333;
];

R = quat2rotm(quaternion(1, 2, 3, 4))

% Define the vector v
x = 1; y = 2; z = 3;
v = [x; y; z];

x1 = x * R(1,1) + y * R(1,2) + z * R(1,3)
y1 = x * R(2,1) + y * R(2,2) + z * R(2,3)
z1 = x * R(3,1) + y * R(3,2) + z * R(3,3)

% Apply the rotation matrix to the vector
v_rotated = R * v;

% Display the rotated vector
disp('The rotated vector is:');
disp(v_rotated);
```