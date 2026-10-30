#!/usr/bin/env python3
"""Generates reddit/diagram.svg: top-view wiring diagram of the SeriousPuck stack.

Usage: python3 make_diagram.py   (then: rsvg-convert -w 2200 diagram.svg -o diagram.png)
"""
from pathlib import Path

W, H = 1120, 900
BW, BH = 178, 210          # XIAO 17.8 x 21 mm, 10 px per mm
PITCH, FIRST = 24, 56

RED, BLK, BLUE, GRN, PUR, ORG = "#d62728", "#333", "#1f77b4", "#2ca02c", "#8e44ad", "#e67e22"

out = []


def add(s):
    out.append(s)


def line(pts, color, w=3, dash=None, dots=()):
    d = " ".join(f"{x},{y}" for x, y in pts)
    extra = f' stroke-dasharray="{dash}"' if dash else ""
    add(f'<polyline points="{d}" fill="none" stroke="{color}" stroke-width="{w}" '
        f'stroke-linejoin="round"{extra}/>')
    for x, y in dots:
        add(f'<circle cx="{x}" cy="{y}" r="4.5" fill="{color}"/>')


def text(x, y, s, size=12, anchor="start", color="#222", weight="normal"):
    add(f'<text x="{x}" y="{y}" font-size="{size}" text-anchor="{anchor}" fill="{color}" '
        f'font-weight="{weight}">{s}</text>')


def gnd(x, y):
    line([(x, y), (x, y + 10)], BLK, 2.5)
    for i, hw in enumerate((12, 8, 4)):
        line([(x - hw, y + 10 + i * 5), (x + hw, y + 10 + i * 5)], BLK, 2.5)


def board(x0, y0, title, subtitle, left, right, used, color):
    add(f'<rect x="{x0}" y="{y0}" width="{BW}" height="{BH}" rx="8" fill="#f3f6fa" '
        f'stroke="{color}" stroke-width="3"/>')
    add(f'<rect x="{x0 + BW / 2 - 26}" y="{y0 - 6}" width="52" height="30" rx="4" fill="#cfd6df" '
        f'stroke="#667" stroke-width="1.5"/>')
    text(x0 + BW / 2, y0 + 15, "USB-C", 10, "middle", "#334")
    text(x0 + BW / 2, y0 + BH + 22, title, 14, "middle", color, "bold")
    text(x0 + BW / 2, y0 + BH + 38, subtitle, 11, "middle", "#555")
    ys = {}
    for i in range(7):
        y = y0 + FIRST + i * PITCH
        ys[i] = y
        for side, label in (("L", left[i]), ("R", right[i])):
            cx = x0 + 7 if side == "L" else x0 + BW - 7
            hot = label in used
            add(f'<rect x="{cx - 7}" y="{y - 5}" width="14" height="10" '
                f'fill="{"#e0b84c" if hot else "#b9bec6"}" stroke="#555" stroke-width="1"/>')
            tx = x0 + 18 if side == "L" else x0 + BW - 18
            text(tx, y + 4, label, 10, "start" if side == "L" else "end",
                 "#000" if hot else "#888", "bold" if hot else "normal")
    return ys


add(f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}" '
    f'font-family="DejaVu Sans, Arial, sans-serif">')
add(f'<rect width="{W}" height="{H}" fill="#fff"/>')
text(W / 2, 32, "SeriousPuck", 20,
     "middle", "#111", "bold")
text(W / 2, 52, "Two XIAO boards stacked: nRF52840 (top) on RP2040 (bottom). Power comes only from the RP2040.",
     12, "middle", "#555")

# ---- boards -------------------------------------------------------------
X0 = 420
NY, RY = 80, 380
nl = ["D0 P0.02", "D1 P0.03", "D2 P0.28", "D3 P0.29", "D4 P0.04", "D5 P0.05", "D6 RX"]
nr = ["5V", "GND", "3V3", "D10 P1.15", "D9 P1.14", "D8 P1.13", "D7 TX"]
rl = ["D0 D+ GP26", "D1 D- GP27", "D2 GP28", "D3 GP29", "D4 GP6", "D5 GP7", "D6 TX"]
rr = ["5V", "GND", "3V3", "D10 GP3", "D9 GP4", "D8 GP2", "D7 RX"]
nused = {"D6 RX", "D7 TX", "5V", "GND"}
rused = {"D0 D+ GP26", "D1 D- GP27", "D2 GP28", "D6 TX", "D7 RX", "5V", "GND"}
ny = board(X0, NY, "XIAO nRF52840", "OpenPuck receiver", nl, nr, nused, "#0a7")
ry = board(X0, RY, "XIAO RP2040", "GP2040-CE", rl, rr, rused, "#c0392b")
LX, RX_ = X0 + 7, X0 + BW - 7

# ---- stack links (same pad on both boards, soldered together) -------------
# left side: D6 <-> D6 (UART RP TX -> nRF RX)
line([(LX, ny[6]), (250, ny[6]), (250, ry[6]), (LX, ry[6])], BLUE, 3, dots=[(LX, ny[6]), (LX, ry[6])])
text(242, 330, "UART  RP TX -&gt; nRF RX", 11, "end", BLUE, "bold")
# right side: 5V, GND, D7 <-> D7 (nested channels)
line([(RX_, ny[0]), (700, ny[0]), (700, ry[0]), (RX_, ry[0])], RED, 3, dots=[(RX_, ny[0]), (RX_, ry[0])])
line([(RX_, ny[1]), (670, ny[1]), (670, ry[1]), (RX_, ry[1])], BLK, 3, dots=[(RX_, ny[1]), (RX_, ry[1])])
line([(RX_, ny[6]), (640, ny[6]), (640, ry[6]), (RX_, ry[6])], BLUE, 3, dots=[(RX_, ny[6]), (RX_, ry[6])])
add('<text transform="translate(628 516) rotate(-90)" font-size="11" text-anchor="middle" fill="#1f77b4" font-weight="bold">nRF TX -&gt; RP RX</text>')
text(706, 140, "5V", 11, "start", RED, "bold")
text(676, 164, "GND", 11, "start", "#000", "bold")
gnd(670, ry[1] + 0)

# ---- Steam Controller -> nRF (radio) --------------------------------------
add('<rect x="40" y="86" width="180" height="70" rx="12" fill="#eef" stroke="#446" stroke-width="2"/>')
text(130, 116, "Steam Controller", 14, "middle", "#223", "bold")
text(130, 136, "(wireless, SC2)", 11, "middle", "#555")
for r in (14, 26, 38):
    add(f'<path d="M {240 + r * 0.2} {121 - r} A {r} {r} 0 0 1 {240 + r * 0.2} {121 + r}" fill="none" '
        f'stroke="{ORG}" stroke-width="2.5"/>')
text(325, 74, "2.4 GHz proprietary radio", 11, "middle", ORG, "bold")

# ---- Xbox console ----------------------------------------------------------
add('<rect x="790" y="285" width="260" height="62" rx="10" fill="#e9f5e9" stroke="#2a6" stroke-width="2"/>')
text(920, 311, "Xbox Series X|S", 14, "middle", "#143", "bold")
text(920, 331, "console USB port (Xbox/GIP output)", 11, "middle", "#555")
line([(X0 + BW / 2, RY - 6), (X0 + BW / 2, 345), (790, 345)], GRN, 4)
text(920, 372, "USB cable to console", 11, "middle", GRN, "bold")
text(920, 386, "(also powers everything)", 11, "middle", "#555")

# ---- USB-A module + original pad ---------------------------------------------
MX, MY = 790, 600
add(f'<rect x="{MX}" y="{MY}" width="110" height="130" rx="6" fill="#dfe6ef" stroke="#556" stroke-width="2"/>')
text(MX + 55, MY + 20, "USB-A", 13, "middle", "#223", "bold")
text(MX + 55, MY + 36, "female", 11, "middle", "#223")
text(MX + 55, MY + 50, "breakout", 11, "middle", "#223")
pins = [("VCC", RED, MY + 75), ("D-", PUR, MY + 95), ("D+", GRN, MY + 115)]
for name, col, y in pins:
    add(f'<rect x="{MX - 7}" y="{y - 5}" width="14" height="10" fill="#e0b84c" stroke="#555"/>')
    text(MX + 12, y + 4, name, 11, "start", "#000", "bold")
add(f'<rect x="{MX - 7}" y="{MY + 135 - 5}" width="14" height="10" fill="#e0b84c" stroke="#555"/>')
text(MX + 12, MY + 139, "GND", 11, "start", "#000", "bold")
gnd(MX - 20, MY + 135)
line([(MX - 20, MY + 135), (MX - 7, MY + 135)], BLK, 2.5)
add('<rect x="960" y="625" width="130" height="80" rx="10" fill="#fdecea" stroke="#b33" stroke-width="2"/>')
text(1025, 652, "Original Xbox", 13, "middle", "#511", "bold")
text(1025, 669, "controller", 13, "middle", "#511", "bold")
text(1025, 689, "(auth relay)", 11, "middle", "#555")
line([(MX + 110, MY + 90), (960, MY + 90)], "#555", 4)

# 5V -> VCC
line([(700, ry[0]), (700, MY + 75), (MX - 7, MY + 75)], RED, 3, dots=[(700, ry[0])])
# D- / D+ from RP D1 / D0 around the left of the stack, under the RP board
line([(LX, ry[1]), (320, ry[1]), (320, MY + 95), (MX - 7, MY + 95)], PUR, 3, dots=[(LX, ry[1])])
line([(LX, ry[0]), (300, ry[0]), (300, MY + 115), (MX - 7, MY + 115)], GRN, 3, dots=[(LX, ry[0])])

# pull-down resistors 15k on D+/D- to GND
for x, y, col, name in ((520, MY + 95, PUR, "D-"), (570, MY + 115, GRN, "D+")):
    line([(x, y), (x, y + 18)], col, 2.5, dots=[(x, y)])
    add(f'<rect x="{x - 6}" y="{y + 18}" width="12" height="30" fill="#fff" stroke="{col}" stroke-width="2"/>')
    line([(x, y + 48), (x, y + 56)], col, 2.5)
    gnd(x, y + 56)
text(680, MY + 205, "15 k pull-downs on D+ / D-", 11, "middle", "#555")
text(680, MY + 219, "(add if the pad is not detected)", 11, "middle", "#555")

# ---- legend ---------------------------------------------------------------------
lx, ly = 60, 640
text(lx, ly, "Legend", 13, "start", "#222", "bold")
for i, (col, name) in enumerate(((RED, "5V (from console via RP2040)"), (BLK, "GND (common)"),
                                 (BLUE, "UART 1 Mbaud 3.3 V"), (GRN, "USB D+"), (PUR, "USB D-"),
                                 (ORG, "radio"))):
    y = ly + 20 + i * 20
    line([(lx, y), (lx + 36, y)], col, 4)
    text(lx + 46, y + 4, name, 11)
text(lx, ly + 198, "nRF USB-C stays UNPLUGGED (RP USB-C feeds both boards).", 11, "start", RED, "bold")
text(lx, ly + 150, "Crossing lines without a dot are NOT connected.", 11, "start", "#555")
text(lx, ly + 166, "Pad order matches the official Seeed XIAO pinouts (USB-C on top).", 11, "start", "#555")

add("</svg>")
Path(__file__).with_name("diagram.svg").write_text("\n".join(out), encoding="utf-8")
