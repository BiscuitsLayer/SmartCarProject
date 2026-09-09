# Smart Car Project

OpenGL driving demo with a Docker runtime and a JSON-driven cinematic exporter.

## Docker

```sh
git clone --branch danila_model https://github.com/BiscuitsLayer/SmartCarProject.git
cd SmartCarProject
git submodule update --init --recursive
docker build -t smartcar:danila-model .
docker run --name smartcar-danila -p 6080:6080 smartcar:danila-model
```

Open `http://localhost:6080/vnc.html?autoconnect=1` to see the 1280x720 application. Use `W/A/S/D` to drive and `Esc` to close it.

## Cinematic export

The application records the OpenGL framebuffer, renders the configured UTF-8 titles, encodes H.264 and muxes the configured audio itself through the linked FFmpeg libraries. It does not launch an external video editor or `ffmpeg` executable.

```sh
mkdir -p output
docker run --rm \
  -e SMARTCAR_CINEMATIC=/app/configs/cinematic.json \
  -v "$PWD/configs/cinematic.json:/app/configs/cinematic.json:ro" \
  -v "/absolute/path/to/music.mp3:/media/vangelis-chariots-of-fire.mp3:ro" \
  -v "$PWD/output:/output" \
  smartcar:danila-model
```

Everything specific to the edit is in [`configs/cinematic.json`](configs/cinematic.json), not in `main.cpp`:

- `export`: output path, FPS, duration, video codec, font, plus the audio path,
  start offset, AAC codec and bitrate. AAC-LC is used by the supplied config for
  reliable iPhone playback.
- `titles`: text, visible interval, fade, size, normalized position and RGB color.
- `shots`: shot interval, visible scene objects, car motion and camera motion.
- A car or camera motion can be `fixed` or `orbit`. Angles and angular speeds are radians/radians per second; car yaw and drift slip are degrees. An orbit can use one `radius` or separate `radius_x`/`radius_z` values for a collision-safe oval.

Changing the JSON is enough to move cuts, add or remove titles, change camera positions, adjust the drift orbit, or select a different part of the soundtrack. No rebuild is needed when the file is bind-mounted as shown above.

## Native Windows run

```powershell
./run.ps1
```

Sport car model: [link](https://sketchfab.com/3d-models/concept-sport-car-566075bdb499404b908895a5f4dc6aa0)

Road model: [link](https://sketchfab.com/3d-models/parking-garage-free-download-5310b7d77b70427d936ec4253fff679c)
