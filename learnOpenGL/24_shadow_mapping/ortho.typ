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

= ortho 正交投影矩阵

== 数学推导

=== 1. X 轴的变换

将视图 $X$ 轴的范围$[l, r]$映射到 $[-1, +1]$ :
- 缩放因子：$S_x = frac(2, r - l)$
- 平移量：$T_x = - frac(r + l, r - l)$
- 变换公式： $x^' = S_x dot x + T_x$

推导公式：

$ x^' = frac(2x - (r + l), r - l) $

=== 2. Y轴的变换
将视图 $Y$ 轴的范围$[b, t]$映射到 $[-1, +1]$ :
- 缩放因子：$S_y = frac(2, t - b)$
- 平移量：$T_y = - frac(t + b, t - b)$
- 变换公式： $y^' = S_y dot y + T_y$

推导公式：

$ y^' = frac(2y - (t + b), t - b) $

=== 3. Z轴的变换
将视图 $Y$ 轴的范围$[n, f]$映射到 $[-1, +1]$ :
- 缩放因子：$S_z = frac(2, f - n)$
- 平移量：$T_z = - frac(f + n, f - n)$
- 变换公式： $z^' = S_z dot z + T_z$

推导公式：

$ z^' = frac(2z - (f + n), f - n) $

=== 4. 正交投影矩阵
组合缩放和平移，得到 4x4 正交投影矩阵：

$ "ortho" =  mat(
  delim: "[",
  S_x, 0, 0, T_x; 0, S_y, 0, T_y; 0, 0, S_z, T_z; 0, 0, 0, 1
) $

上面的矩阵展开就是：

$ "ortho" = mat(
  delim: "[",
  frac(2, r - l), 0, 0, -frac(r + l, r - l); 0, frac(2, t - b), 0, -frac(t + b, t - b);
  0, 0, frac(2, f-n), -frac(f + n, f - n); 0, 0, 0, 1
) $
