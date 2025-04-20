# Game Engine Vulkan - TODO List

## Hoàn thành
- [x] Load model định dạng glTF
- [x] Hỗ trợ animation cơ bản (skeletal animation)
- [x] Hiển thị thông tin FPS và tốc độ xử lý
- [x] Đổi màu ambient và directional (sun) light
- [x] Debug info: render type, performance data
- [x] Camera (movement, control, switching)
- [x] Rendering pipeline cơ bản
- [x] Giao diện ImGui

---

## Core Engine
- [ ] Culling (Frustum Culling, Occlusion Culling)
- [ ] LOD (Level of Detail) cho mesh
- [ ] Scene graph (cây quản lý đối tượng và hierarchy)
- [ ] Entity Component System (ECS)

---

## Rendering
- [ ] Shadow Mapping (Directional, Point light)
- [ ] PBR nâng cao (Specular-Glossiness, IBL)
- [ ] Post-processing effects:
  - [ ] Bloom
  - [ ] Tone Mapping
  - [ ] SSAO / SSR (Screen Space Effects)
- [ ] Particle system
- [ ] Skybox / HDR environment map
- [ ] Multiple light sources (nhiều ánh sáng dynamic)

---

## Gameplay Systems
- [ ] Input mapping (configurable keybindings)
- [ ] Physics integration (Bullet / PhysX)
- [ ] Trigger, collider và interaction logic

---

## Tooling & Asset Pipeline
- [ ] Pipeline convert định dạng model khác → glTF
- [ ] Scene editor (gắn vào ImGui): kéo thả object, chỉnh ánh sáng, transform
- [ ] Hot-reload shader và asset
- [ ] Debug view: normal map, wireframe, light map,...

---

## Test & Debug
- [ ] Test các corner case của animation & transform
- [ ] Kiểm tra memory leak (valgrind / sanitizer)
- [ ] Debug bounding box, AABB, skeleton

---

## Scene & Project System
- [ ] Lưu scene (transform, mesh, light...) vào file (JSON/YAML/Binary)
- [ ] Tự động load & serialize state
- [ ] Config hệ thống render, resolution, VSync,...

---
