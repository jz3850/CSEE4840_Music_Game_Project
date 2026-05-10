# Rhythm Master

基于 Intel DE1-SoC FPGA 平台实现的嵌入式音游（Rhythm Game）系统。  
项目结合 FPGA + HPS（ARM）协同设计，实现了实时音频播放、VGA 图形渲染、USB 键盘交互以及完整的节奏游戏逻辑。

---

# UI logo

<img width="300" height="230" alt="image" src="https://github.com/user-attachments/assets/c24fba03-9479-4e2a-894d-11c4819c57a9" />


# 项目简介

Rhythm Master 是一个运行在 Intel DE1-SoC FPGA 平台上的节奏音乐游戏。

玩家通过 USB 键盘输入，与屏幕中下落的音符进行同步击打。系统基于 FPGA + HPS 的软硬件协同架构，实现：

- 实时音频播放
- VGA 图形渲染
- 音频与画面同步
- 音符判定与评分
- 菜单与暂停系统
- Sprite 动画渲染

整个系统强调：

- 实时性
- 低延迟
- FPGA/HPS 协同设计
- 有限 BRAM 资源优化
- 高效视频更新机制

---

# 系统架构

<img width="1200" height="410" alt="image" src="https://github.com/user-attachments/assets/a58bdca2-9708-42a2-ab27-1e5c28dd9d6b" />

系统由 FPGA 与 HPS 两部分组成：

## FPGA 部分

负责：

- Audio Core 音频输出
- WM8731 Codec 配置
- VGA 时序生成
- Framebuffer 管理
- Color LUT
- Avalon-MM 接口

## HPS 部分

负责：

- 游戏逻辑
- USB 键盘输入
- Sprite 渲染
- 音频数据流
- Frame Diff 更新
- UI 菜单系统

音频与图像通过 Avalon-MM 接口实现 FPGA 与 HPS 的通信。 

---

# 核心特性

- FPGA + HPS 协同架构
- VGA 实时图形渲染
- 音频与画面同步
- USB 键盘输入
- Sprite 动画系统
- 差分 Framebuffer 更新
- 音符判定系统
- Combo 与评分系统
- Pause Menu
- 多难度支持

---

# 硬件设计

## 音频系统

系统基于 Intel Audio Core IP + WM8731 Codec 实现音频播放。

音频数据由 HPS 实时流式传输至 FPGA，再通过 I²S 输出至 Codec。 

### 音频硬件结构图

<img width="550" height="330" alt="image" src="https://github.com/user-attachments/assets/07e0c065-8c30-452c-bc26-0d1a2643250b" />

### Audio Platform Designer 配置

<img width="1200" height="650" alt="image" src="https://github.com/user-attachments/assets/6ccf97d6-e053-4c45-8129-1c969fe4daf7" />

---

## VGA 显示系统

VGA 显示模块由自定义 SystemVerilog 模块实现：

- VGA 时序生成
- Framebuffer 管理
- LUT 颜色映射
- 差分像素更新

系统仅存储：

```text
8-bit Color Index
```

而不是完整 RGB 数据，从而显著降低 FPGA BRAM 占用。 :contentReference[oaicite:3]{index=3}

### VGA Framebuffer 结构图

<img width="700" height="330" alt="image" src="https://github.com/user-attachments/assets/9b7c2fab-a71c-4a3d-8291-fcaa49d9969f" />

### VGA Platform Designer 配置

<img width="1200" height="400" alt="image" src="https://github.com/user-attachments/assets/eabd986c-c759-4c14-950d-aa2fa53ffca3" />

---

# 软件架构

系统软件运行于 HPS（ARM Cortex-A9）。

主要模块包括：

| 模块 | 功能 |
|---|---|
| Audio Thread | 音频流播放 |
| Sprite Renderer | 图像渲染 |
| Video Renderer | Framebuffer 更新 |
| Game Logic | 游戏逻辑 |
| Input Handler | USB 键盘输入 |

---

# Audio Playback

音频系统采用独立线程实现：

- FIFO 空间检测
- PCM 数据流发送
- 音频时间同步
- 双缓冲机制

音频播放时间作为整个游戏系统的同步基准。 

### Audio Thread Flowchart

<img width="400" height="456" alt="image" src="https://github.com/user-attachments/assets/ccd03f70-e32c-42f6-be4e-f410e57a7f6b" />

---

# Sprite Rendering

Sprite 系统支持：

- PNG 素材
- Transparency
- Palette 压缩
- Sprite Animation
- Layer Rendering

项目使用 Python 工具链将原始 PNG 图片转换为适合 FPGA 的 16-color Palette 格式。 

---

# Diff-based Framebuffer Rendering

系统维护：

```text
current_frame
next_frame
```

渲染完成后：

- 对比两帧
- 仅发送变化像素
- 通过 Avalon-MM 更新 FPGA

这种机制显著降低了 HPS 与 FPGA 间的数据传输量。 

---

# 游戏逻辑

游戏系统实现：

- Note Spawn
- Hit Detection
- Combo
- Scoring
- Pause Menu
- End Game Evaluation

判定包括：

| Result | Description |
|---|---|
| Perfect | 精准命中 |
| Good | 部分命中 |
| Miss | 未命中 |

评分系统：

```text
Perfect : +50
Good    : +30
Combo   : combo × 5
```

最终根据分数计算：

```text
S / A / B / C / D
```

等级。 

### Game Logic Flowchart

<img width="800" height="730" alt="image" src="https://github.com/user-attachments/assets/50661309-435b-4fdb-a0d3-9ae9f0da9f03" />

---

# Sprite Resources

<img width="600" height="770" alt="image" src="https://github.com/user-attachments/assets/34b3abec-d136-4299-a14e-8e95e1deee0d" />

项目包含：

- Background
- Notes
- Combo
- Hit Feedback
- Level Rating
- Number Fonts

等图形资源。 

---

# BRAM 优化

Cyclone V FPGA BRAM 容量有限：

```text
≈ 556 KB
```

若直接存储：

```text
640×480×24bit RGB
```

需要：

```text
≈ 902 KB
```

超出 FPGA BRAM 容量。

因此项目采用：

```text
8-bit Color Index + LUT
```

方案，将 Framebuffer 压缩至：

```text
≈ 307 KB
```

成功运行于 FPGA BRAM 中。 

---

# 项目目录结构

```text
hardware/
├── vga_framebuffer.sv
├── soc_system_top.sv
└── PlatformDesigner/

software/
├── game.c
├── sprite.c
├── framebuffer.c
├── audio_user.h
├── usbkeyboard.c
└── fbputchar.c
```

---

# 使用硬件

| 硬件 | 型号 |
|---|---|
| FPGA Board | Intel DE1-SoC |
| FPGA | Cyclone V |
| Audio Codec | WM8731 |
| Display | VGA Monitor |
| Input | USB Keyboard |

---

# 技术栈

- SystemVerilog
- C
- Linux HPS
- FPGA BRAM
- Avalon-MM
- VGA
- I²S
- USB Keyboard
- Intel Platform Designer

---

# 项目亮点

- FPGA/HPS 协同设计
- VGA Framebuffer 自定义实现
- Diff-based Rendering
- 实时音频同步
- BRAM 优化
- Color LUT 图形压缩
- Sprite 动画系统
- 完整音游逻辑实现

---

# 项目成员

| Name | Contribution |
|---|---|
| Yangyang Zhang | FPGA Audio Pipeline / VGA Hardware / Frame Diff System |
| Junfeng Zou | Game Logic / Input /  Sprite Rendering / Synchronization |

---

# 项目总结

本项目展示了：

- 实时嵌入式系统设计
- FPGA 与 ARM 协同开发
- 多媒体系统实现
- 图形与音频同步
- 有限资源优化

等核心 Embedded Systems 能力。 

---


