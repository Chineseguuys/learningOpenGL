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

= Wayland 桌面环境下使用Renderdoc 抓取程序绘制

我使用的 Manjaro Linux KDE 使用的是 Wayland, 目前 Renderdoc没有对 wayland 的支持，导致在使用 Renderdoc 去抓帧， 无法正常启动程序,或者启动程序后，点击抓取按钮没有响应

== 暂时的解决方案

在 Renderdoc 中启动程序的时候, 在 Environment Variables 中添加额外的变量值:
$ "XDG_SESSION_TYPE" = "x11" $

#figure(
  image("imgs/1.png"),
  caption: "RenderDoc 启动选项设置"
)

之后再 Launch 应用就能够正常的抓取应用的绘制了。

== 参考链接
#link("https://github.com/baldurk/renderdoc/issues/853")[
  #text(blue)[#underline("Wayland support on Linux #853")]
]

#link("https://www.reddit.com/r/vulkan/comments/167jh7l/renderdoc_vulkan_extensions_on_linux/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button")[
  #text(blue)[#underline("RenderDoc Vulkan Extensions on Linux")]
]