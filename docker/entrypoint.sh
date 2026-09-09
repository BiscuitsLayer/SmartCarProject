#!/bin/sh
set -eu

export DISPLAY=:99
Xvfb "$DISPLAY" -screen 0 1280x720x24 -ac +extension GLX +render -noreset &
xvfb_pid=$!
trap 'kill "$xvfb_pid" "$vnc_pid" "$web_pid" 2>/dev/null || true' EXIT INT TERM

while ! xdpyinfo -display "$DISPLAY" >/dev/null 2>&1; do
    sleep 0.1
done

x11vnc -display "$DISPLAY" -forever -shared -nopw -quiet -rfbport 5900 &
vnc_pid=$!
websockify --web=/usr/share/novnc 6080 localhost:5900 &
web_pid=$!

exec ./SmartCarMain "${SMARTCAR_CONFIG:-/app/config.json}"
