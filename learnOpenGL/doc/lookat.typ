// 设置页面的大小和边距，A4 大小，页面的左边距为 1.8， 上下边距为 1.5cm
#set page(
  paper: "a4",
  margin: (x: 1.8cm, y: 1.5cm),
)

// 设置页面文字的大小
#set text(
  font: ("Noto Sans Kaithi"),
  lang: "zh",
  weight: "regular"
)

= glm::lookAt 函数

glm::lookAt() 函数用于构建视图矩阵（View Matrix），将世界坐标系中的点转换到相机坐标系。以下是其数学推导过程的分步解释：

== 1. 输入参数
函数原型为：

```cpp
glm::mat4 lookAt(
    glm::vec3 eye,    // 相机位置
    glm::vec3 center, // 目标点（相机对准的位置）
    glm::vec3 up      // 上方向向量（通常为(0,1,0)）
);
```

== 2. 坐标系定义
- 相机坐标系：相机位于原点，看向 $-z$ 方向，$x$ 向右，$y$ 向上（右手坐标系）。
- 目标：将世界坐标系中的点变换到相机坐标系。


== 3. 推导步骤

=== (1)计算前向向量（Forward Direction）
从相机位置指向目标点的方向向量，并归一化：

$ arrow("f") = norm(arrow("center") - arrow("eye")) $

在相机坐标系中，前向方向为 $-z$，因此实际使用的前向向量是 $- arrow(f)$。

=== (2) 计算右向量（Right Direction）
使用叉乘计算相机的右方向（x轴）：

$ arrow("r") = norm(arrow("f") times arrow("up")) $

注意：叉乘顺序需符合右手法则（OpenGL使用右手系）。

=== (3) 计算上向量（Up Direction）

通过右向量和前向向量重新计算正交化的上方向：

$ arrow("u") = norm(arrow("r") times arrow("f")) $

=== (4) 构造旋转矩阵

将相机坐标系对齐到世界坐标系。旋转矩阵的列向量为相机的右、上、前方向：

$ "R" = mat(
  delim: "[",
  arrow(r)_x, arrow(u)_x, -arrow(f)_x, 0; arrow(r)_y, arrow(u)_y, -arrow(f)_y, 0;
  arrow(r)_z, arrow(u)_z, -arrow(f)_z, 0; 0, 0, 0, 1
) $

由于前向向量实际是 $-arrow(f)$，因此在矩阵中使用负值。

=== (5) 构造平移矩阵
将相机位置平移到原点：

$ "T" = mat(
  delim: "[",
  1, 0, 0, -arrow("eye")_x; 0, 1, 0, -arrow("eye")_y; 0, 0, 1, -arrow("eye")_z; 0, 0, 0, 1
) $

=== (6) 组合旋转和平移

视图矩阵是 先旋转再平移，即：

$ M_"view" = R dot T $

展开后为：

$ M_"view" = mat(
  delim: "[",
  arrow(r)_x, arrow(u)_x, -arrow(f)_x, -arrow(r) dot arrow("eye");
  arrow(r)_y, arrow(u)_y, -arrow(f)_y, -arrow(u) dot arrow("eye");
  arrow(r)_z, arrow(u)_z, -arrow(f)_z, arrow(f) dot arrow("eye");
  0, 0, 0, 1
) $

其中平移分量通过点乘计算，将相机的世界坐标转换到新坐标系。

== 4. 代码实现

GLM 的实现与上述推导一致：

```cpp
template<typename T, qualifier Q>
GLM_FUNC_QUALIFIER mat<4, 4, T, Q> lookAtRH(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
{
	vec<3, T, Q> const f(normalize(center - eye));
	vec<3, T, Q> const s(normalize(cross(f, up)));
	vec<3, T, Q> const u(cross(s, f));

	mat<4, 4, T, Q> Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] =-f.x;
	Result[1][2] =-f.y;
	Result[2][2] =-f.z;
	Result[3][0] =-dot(s, eye);
	Result[3][1] =-dot(u, eye);
	Result[3][2] = dot(f, eye);
	return Result;
}
}
```

