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

= glm::perspective()

== 1.视锥体参数

- #strong("垂直视野角(fovy)"): 定义视锥体的垂直开口角度;
- #strong("宽高比(aspect)"): 视口的宽高比（宽度/高度）;
- #strong("近平面(n) 和 远平面(f)")：定义视锥体的深度范围;

近平面的高度和宽度由以下公式确定：

$ h = 2n dot tan(frac("fovy", 2)), w = "aspect" dot h $

== 2.投影变换目标
将视锥体映射到 NDC 立方体 $[-1, 1]^3$, 通过下面的步骤:
- 缩放 x 和 y: 根据宽高比和视野角度去调整坐标的范围
- 处理深度z: 将 z 从 $[-n, -f]$ 映射到 $[-1, 1]$ 或者 $[0, 1]$
- 透视除法: 通过齐次坐标的 $w$ 分量完成非线性的映射

== 3.矩阵推导
=== x 和 y 分量的缩放
根据相似三角形，投影后坐标需满足：

$ x^' = frac(x dot n, -z dot w / 2), y^' = frac(y dot n, -z dot h / 2) $

引入缩放因子，使 x 和 y 范围映射到 $[-1, 1]$:

$ "scale"_x = frac(1, "aspect" dot tan(frac("fovy", 2))), "scale"_y = frac(1, tan(frac("fovy", 2))) $

对应矩阵前两行：

$ mat(
  delim: "[",
  "scale"_x, 0, 0, 0; 0, "scale"_y, 0, 0
) $

=== z 分量
将 z 从 $[-n, -f]$ 映射到 $[-1, 1]$, 设变换后的 z 为:
$ z^' = frac(A dot z + B, -z) $
通过边界条件 $z = -n arrow z^' = -1$ 和 $z = -f arrow z^' = 1$, 可以计算得到:
$ A = -frac(f + n, f - n), B = -frac(2f n, f - n) $

=== w 分量的处理
设置 w 分量为 $-z$ 确保透视除法正确:
$ w^' = -z arrow.r.double [0, 0, -1, 0] $

== 4. 最终矩阵
组合所有分量，得到透视投影矩阵：

$ mat(
  delim: "[",
  frac(1, "aspect" dot tan("fovy" / 2)), 0, 0, 0;
  0, frac(1, tan("fovy" / 2)), 0, 0;
  0, 0, -frac(f+n,f-n), -frac(2f n, f-n);
  0, 0, -1, 0
) $
