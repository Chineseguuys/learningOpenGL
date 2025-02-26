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


= Show YUV Frame by OpenGL

== HLG 参考 OETF

$ E^' = "OETF"[E] = cases(
  sqrt(3 E) "                       " 0 lt.eq.slant E lt.eq.slant frac(1, 12),
  a dot ln(12 E - b) + c "     " frac(1, 12) lt E lt.eq.slant 1
) $

其中： E 与场景线性光成比例的每一个颜色分量的 ${R_s, G_s, B_S}$ 的信号，归一化的范围为 $[0, 1]$。
$E^'$ 是 $[0,1]$范围内产生的非线性信号。

$a=0.17883277,b = 1 - 4 a = 0.28466892, c = 0.5 - a ln(4 a) = 0.55991073$

== HLG 参考 OOTF

#align(center)[
  $F_D = "OOTF"[E] = alpha Y_S^(gamma - 1) E$ \
  $R_D = alpha Y_S^(gamma - 1) R_S$ \
  $G_D = alpha Y_S^(gamma - 1) G_S$ \
  $B_D = alpha Y_S^(gamma - 1) B_S$ \
  $Y_S = 0.2627 R_S + 0.6780 G_S + 0.0593 B_S$
]

其中，

$F_D$ 是显示的线性分量 ${R_D, G_D, B_D}$ 的亮度,单位为 $"cd"slash m^2$。

$E$ 是与场景线性光线成比例的每一个颜色分量 ${R_S, G_S, B_S}$ 的信号，并通过相继曝光缩放，归一化到范围 $[0,1]$。

$Y_S$ 是归一化的线性场景的亮度。

$alpha$ 是以 $"cd"slash m^2$为单位的用户增益变量。它代表了$L_W$， 是显示器消色差像元的标称峰值亮度;

$gamma$ 为系统伽马射线。显示器标称亮度为 $1000 "cd"slash m^2$ 时，$gamma$ 的值为 1.2。


== links

#link("https://isovideo.com/index.php")[
  #text(blue)[#underline("sovideo.com")]
]

#link("https://github.com/IENT/YUView")[
  #text(blue)[#underline("YUView on Github")]
]

#link("https://4kmedia.org/")[
  #text(blue)[#underline("4kmedia")]
]
