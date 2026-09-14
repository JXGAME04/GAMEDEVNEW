# PHÂN TÍCH NHỊ PHÂN `D:\game3gTQ` (剑网江湖 · Kiếm Võng Giang Hồ V24.2) — CÁCH VẼ HÌNH VÀ CAMERA 3D QUAY

> Chủ 13/09: *"đọc dữ liệu D:\game3gTQ xem cách họ vẽ hình ảnh trong game · cần đọc nhị phân toàn bộ rồi phân tích không fix code · cần nhất phần 3d quay camera"*.
> Chỉ phân tích, **không đụng mã JX1**. Toàn bộ tệp bung/công cụ/kết quả trung gian nằm ở `D:\game3gTQ_mo` (thư mục làm việc mới, tệp gốc trong `D:\game3gTQ` không đổi).

## 0. Kết luận cho JX1 mobile (đọc trước)

1. **Đây là game Unity 3D thật** (Unity 2022.3.62f3c1 bản Trung Quốc, IL2CPP, URP): nhân vật là mesh có xương, cảnh là mesh + lightmap, camera là `Camera` phối cảnh FOV 40 quay quanh nhân vật. Không có gì là sprite 2D 8 hướng như JX1. Vì vậy **phần "quay camera" của họ không chép được sang JX1** — kết luận cũ vẫn đúng (sprite 8 hướng, nền vẽ sẵn phẳng, xem ghi nhớ `zoom-canh-mobile-phan-tich`). Cái chép được là **cách điều khiển và cách "cảm giác"** (mục 6), không phải phép quay.
2. Camera của họ là **quả cầu quanh nhân vật với 3 số: yaw (`mAngleX`), pitch (`mAngleY`), khoảng cách (`mDistance`)**. Vị trí = mục tiêu + Rot_y(yaw) · (0, d·sin(pitch), −d·cos(pitch)), rồi `LookAt(mục tiêu)`. Công thức đầy đủ ở mục 3.2.
3. Số liệu thật của bảng cảnh (`scn_list.cameraInit`): **36/65 cảnh dùng `19*10*21*0*40*40*80`** = khoảng cách 19 (kéo gần 10, xa 21), yaw 0, pitch 40°, **pitch kẹp 40°–80°**. Tức là gần như chỉ được quay ngang, nhìn xuống khá gắt; không cho lật ngang tầm mắt.
4. Điều khiển (giống nhau trên PC và Android, mã hai bản **giống hệt** — đã so metadata): một ngón kéo trên vùng không phải UI = quay; hai ngón = zoom; PC chuột phải kéo = quay, con lăn = zoom; sau khi thả tay **chờ 1 s** rồi tự trôi yaw về sau lưng nhân vật với tốc độ 40°/s (`Mathf.MoveTowardsAngle`); cần trái/WASD đi theo **hướng tương đối camera**.
5. Bộ công cụ tôi viết (mục 8) mở được **toàn bộ** dữ liệu của họ: metadata IL2CPP, địa chỉ 55 809 hàm, dịch ngược có chú thích tên trường, **khoá giải mã bundle** (PC `MLL8Fps15BNaFvsh`, Android `DcmWEdJRL448nU6m`), 68 bundle, 237 kịch bản Lua dạng chữ, bảng dữ liệu. Dùng lại được cho bất kỳ câu hỏi nào khác về game này.

## 1. Gói dữ liệu là gì

| | PC (`PC.Kiếm võng Bản Pc _V24.2.zip`, 1,56 GB) | Android (`Android.Kiếm võng bản android _V24.2.APK`, 1,62 GB) |
|---|---|---|
| Engine | Unity **2022.3.62f3c1** (c1 = bản Unity China), IL2CPP, Release, `StripEngineCode:0` | như PC; `arm64-v8a` + `armeabi-v7a`; `androidRenderOutsideSafeArea=1` |
| Mã game | `GameAssembly.dll` 41,5 MB + `global-metadata.dat` 8,2 MB (**không mã hoá**, v31) | `libil2cpp.so` 49 MB + metadata 8,1 MB (không mã hoá) |
| Assembly-CSharp | 1 772 lớp, 55 module; Lua: **uLua/tolua Lua 5.1** (`ulua.dll` có `lua_getfenv`, `lua_cpcall`) + lớp bọc `LuaReg_*` tự viết | giống PC (chỉ khác 2 lớp: thiếu `NativeWindowUtils`, thêm `ApolloVoice_lib`, `Proto`) |
| Pathfinding | `pathfind.dll` = **GXAI** (`GXAIScene_GetNavMeshIndexBuffer`, `GXAIContext_AStartTest`, `AIGrid`) — navmesh + lưới A* riêng, dữ liệu `AIS\0` trong bundle | `libpathfind.so` |
| Tài nguyên | 68 tệp `.bdd` = **UnityFS** (Unity 2022.3) trong `StreamingAssets`, **mã hoá bằng cơ chế AssetBundle encryption có sẵn của Unity** (cờ 0x200, chữ ký `#$unity3dchina!@`) | cùng tên tệp, cùng cấu trúc, **khoá khác** |
| UI | **NGUI** (UICamera, UIPanel, UISprite…) + TextMeshPro; HTML engine cho chữ | như PC |
| Khác | URP có tuỳ biến (`ScnRenderPipeline`, `GrabPassFeature`, `GUILowFeature`, `ModelViewFeature`), DynamicShadowProjector, T4M terrain, XWeaponTrail, PigeonCoop trails, Apollo voice, TSS anti-cheat (`TssSdt*`) | |

Hãng: `Jstbb Software Ltd.`; tên gói `剑网江湖`; dựng 06/09/2026.

## 2. Cách họ vẽ hình (tổng quan pipeline)

**Cảnh (46 bundle cảnh, mỗi cảnh một bundle):** GameObject `MainCamera` (Camera FOV 40, near 1, far 500 ở 20 cảnh; far 100/80/50 ở cảnh nhỏ; + `UniversalAdditionalCameraData`, có cảnh thêm `CullDistances` = khoảng cách cull theo layer) + hàng nghìn `MeshFilter/MeshRenderer` (tổng 33 740 MeshFilter, 6 094 Mesh; 720 mesh ≥ 10 k đỉnh) + `Scene_Ref` (774 cái: bảng tham chiếu `mData[]/mType[]/mIndex[]/mLightmap[]` = cách họ nén "cảnh" thành danh sách prefab + toạ độ lightmap) + `SceneRenderSetting/SceneConfiger` (ambient, fog, shadow distance/quality, `_useMazeFog`) + lightmap nướng sẵn (`Lightmap-N_comp_light` 1024²/2048², **BC6H** trên PC, **ASTC HDR 4x4** trên Android) + cubemap skybox (`skycube_tex_N`). Terrain = **T4M** (Terrain For Mobile: shader `地形_2张控制图8贴图` = 2 control map × 8 texture, `T4MPlantObjSC` 6 427 cụm cỏ/cây, LOD + billboard theo khoảng cách tới `PlayerCamera`). Nước: shader `水体高光双浪` (+ bản "鸟瞰图专用" cho bản đồ nhìn từ trên).

**Nhân vật:** mesh có xương lắp từ mảnh: `ChaResourceRef{meshUrl, matUrl, clothUrl, bones[]}` → `AssetPool_Skin` gộp mesh (`cacheCombineMesh`, `cacheBones`, `Cloth`) → 1 `SkinnedMeshRenderer`/nhân vật; trang bị treo qua `HangItemMgr` (124) ; hoạt ảnh **legacy `Animation`** (253) với clip đặt tên `xx01` (đứng), `xdz01` (đi), `gj01/gj02` (đánh), `ss01` (bị đánh), `sw01` (chết), `zp01`… ; shader nhân vật `角色/cha_base[_rim|_spec|_alphatest|_transparent]`. Bóng nhân vật = **DynamicShadowProjector** (Projector orthographic, RT 6 hoặc 4 đơn vị) + `ShadowProjMgr`; bóng cảnh = shadow map URP (`SceneConfiger.SetShadowDistance/Quality`).

**Hiệu ứng:** 315 ParticleSystem, `SFXBillboardHelper` (128, billboard nhiều kiểu: Billboard / RotBillboardY / Horizontal / Vertical …), `SFXMeshModify` (490), `SFXXWeaponAnim/Anchor` (vệt kiếm XWeaponTrail), `SFXMixerMesh`, shader `particle/blend_dst_*`, `mesh_trail_drag_*`, `sfx_distort_feature` (méo hình qua `GrabPassFeature` copy màn hình).

**Hậu kỳ:** URP Volume (Bloom + Tonemapping), `CameraBlur` (radial), `CameraFade`, `GUILowFeature` (vẽ lớp UI thấp + `ModelView_BGTex`), `ModelViewFeature` (render nhân vật vào RT `model_view` cho UI), RT `map`.

**Camera xếp chồng (URP camera stack):** base = MainCamera thế giới; overlay = camera UI NGUI `ui_root/Camera`, `CameraLow` (`AddToMainCameraStack.SortStack/ExeMainCameraStack`, `MainCameraStackStartInit`).

**Kết cấu:** PC DXT1 382 / BC7 325 / DXT5 136 / BC6H 124 / RGBA32 102; Android **ASTC 6x6** (214) + **ASTC 4x4** (194) trong cùng bundle kết cấu 117 MB (`3055752669ff`, 410 texture, 512² chiếm đa số).

**Dữ liệu chữ:** 898 TextAsset: 237 `.lua` **dạng chữ, không mã hoá** (logic UI/scene/skill: `scn_area.lua`, `ui_primary_rocker.lua`…), bảng số liệu = JSON mô tả cột (`scn_list`) + TSV UTF-16 (`scn_list_cmn`, `scn_area_list`…), dữ liệu đường đi `AIS\0` (`world_bianjing`…), `ab_name_map` 1 MB (ánh xạ tên → bundle băm, có mã hoá riêng, chưa cần mở).

## 3. Camera 3D — mổ từ mã (`GameCamera : EventCamera : MonoBehaviour`)

### 3.1 Trường quan trọng (offset trong đối tượng, đã đối chiếu khi dịch ngược)

| Trường | Ý nghĩa |
|---|---|
| `mTargetTrans` / `mCrtTarget` (Creature) | mục tiêu bám (nhân vật; đổi khi cưỡi: `RideUnit.CreateRide/SetDriver`) |
| `HeightOffset`, `Offset` | điểm nhìn = vị trí nhân vật + `Offset`, với `Offset.y = HeightOffset` (`get_targetPos`) |
| `mDistance`, `mWheelDistance`, `mMinDistance`, `mMaxDistance`, `fDistanceSpeed`, `fWheelSpeed` | khoảng cách hiện tại / khoảng cách đích do con lăn-pinch / kẹp / tốc độ nội suy / tốc độ zoom (= `GameConfig` json ÷ 50) |
| `mAngleX` (yaw), `mAngleY` (pitch), `minAngleY`, `maxAngleY`, `angleSpeedRate` | góc và kẹp pitch, hệ số nhạy kéo |
| `AngleAutoFollow`, `AngleAutoFollowSpeed` (=40), `AngleAutoFollowAngleY`, `fTouchWaitTime` (=1 s sau khi thả), `bTouch` | tự trôi về sau lưng nhân vật |
| `UseFirstAngle`, `ForceFirstAngleFollow`, `FirstAngleFollowSpeed`, `bFollowSmooth` | chế độ khoá yaw theo hướng nhân vật (kiểu "góc thứ nhất") |
| `EventCamera`: `mTask` (ITask: `TweenCamera`/`CameraAnim`), `mShakeParams`, `bMayControlMove/Angle/Wheel`, `mTouchControl` | tác vụ chuyển cảnh, rung, khoá điều khiển, bộ nhận chạm |

### 3.2 Mỗi khung (`LateUpdate` → `InternalUpdate` → `OnUpdate` → `UpdateCameraParam`) — dựng lại từ mã máy

```
// EventCamera.InternalUpdate
if (mTask != null && !mTask.RenderTick()) { mTask.Exit(); mTask = null; }   // TweenCamera / CameraAnim chạy TRƯỚC
if (shake.time > 0) { shake.time -= dt; if (shake.time < 0) localPosition = shake.saved;
                      else if (unscaledTime >= shake.next) { shake.next += period; localPosition += Random.insideUnitSphere * swing; } }

// GameCamera.UpdateCameraParam (chỉ khi !pauseUpdate && mTargetTrans != null)
if (UseFirstAngle) {                                   // khoá yaw theo nhân vật
    if (!bTouch || ForceFirstAngleFollow) {
        cur = Repeat(mAngleX,360); tgt = Repeat(target.eulerAngles.y,360);
        if (|cur-tgt| > 180) { if (tgt > cur) cur += 360; else tgt += 360; }      // đường ngắn nhất
        if (!bFollowSmooth) mAngleX = tgt;
        else { k = clamp01(FirstAngleFollowSpeed); mAngleX = cur + (tgt-cur)*k; if (|mAngleX-tgt| < 0.05) bFollowSmooth = false; }
    }
} else if (bTouch && fTouchWaitTime > 0) fTouchWaitTime -= dt;          // đang/vừa kéo: không tự trôi
else if (AngleAutoFollow && bMayControlAngle) {
    step = dt * AngleAutoFollowSpeed;                                         // 40°/s
    mAngleX = MoveTowardsAngle(mAngleX, target.eulerAngles.y, step);
    mAngleY = MoveTowardsAngle(mAngleY, AngleAutoFollowAngleY, step);
}
mWheelDistance = clamp(mWheelDistance, mMinDistance, mMaxDistance);
t = clamp01(dt * fDistanceSpeed);
mDistance = clamp(mDistance + (mWheelDistance - mDistance) * t, mMinDistance, mWheelDistance);   // zoom mượt
Vector3 tp = targetPos;                                                        // nhân vật + (Offset.x, HeightOffset, Offset.z)
Vector3 off = Quaternion.Euler(0, mAngleX, 0) * new Vector3(0, mDistance*sin(mAngleY*Deg2Rad), -mDistance*cos(mAngleY*Deg2Rad));
mLastAngleX = mAngleX; mLastAngleY = mAngleY;
transform.position = tp + off;
transform.LookAt(tp);
CameraBuildingFade.Update(tp + off, tp);                                      // làm mờ nhà che (raycast + bounds, keyword shader)
if (staticFlag) Shader.SetGlobalVector(SHADER_PLAYERPOS /* "PlayerPos" */, tp);  // shader cỏ/cây/nước biết vị trí người chơi
// rồi OnUpdate tick tiếp UIFollow3DWorld / UIFollowTarget3D / UIFollowTarget (chữ, thanh máu neo 3D) + HeadTopPanel.ManualLateUpdate
```

Yaw 0 = camera đứng phía **−Z** của nhân vật nhìn về +Z; pitch tính từ mặt phẳng ngang (40° = nhìn xuống 40°). Không có va chạm camera với tường (không thấy SphereCast/raycast kéo camera lại) — họ giải quyết bằng **làm mờ vật che** (`CameraBuildingFade`: `RefreshOccluders` theo chu kỳ `DetectInterval`, `CollectByRaycast/CollectByBounds`, `ApplyAlpha` với `FadeSpeed`, bật keyword shader lúc khởi động qua `RuntimeInitializeOnLoad`).

### 3.3 Khởi tạo và số liệu thật

`GameControl.Init(Player)`: gắn `GameCamera` vào `MainCamera` của cảnh (prefab cảnh **không** chứa `GameCamera` — đã kiểm 53 Camera trong bundle, chỉ có `Camera + UniversalAdditionalCameraData [+ CullDistances]`), rồi đọc `mbtb_scn_list.cameraInit` (float[7]):

| chỉ số | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|---|
| ý nghĩa (chú thích bảng) | 距离 khoảng cách | 最小距离 | 最大距离 | X角度 yaw | Y角度 pitch | 最小Y角度 | 最大Y角度 |
| gán vào | `mDistance` **và** `mWheelDistance` | `mMinDistance` | `mMaxDistance` | `mAngleX` | `mAngleY` | `minAngleY` | `maxAngleY` |
| mặc định khi bảng trống | 20 | 0,1 | 50 | 0 | 45 | 0 | 89 |

Giá trị trong `scn_list_cmn` (65 cảnh): `19*10*21*0*40*40*80` ×36 (巴陵县, 永乐镇, 稻香村, 成都府, 临安府…), `19*10*20*166*40*40*80` ×5, `19*10*20*-150*40*40*80` ×4, `19*10*20*-10*40*40*80` ×3, `23*15*25*160*45*40*80`, `19*10*30*0*40*0*88` (một cảnh cho lật tới 0°)… → thiết kế **"nhìn xuống 40–80°, zoom 10–21"**, yaw đầu tuỳ cảnh (166°, −150° … để mở màn đúng hướng đẹp). `GameCamera.OnStart` thêm: nếu khoảng cách < 0,001 thì 5; `AngleAutoFollowSpeed = 40`; `fWheelSpeed = cfg/50`; lưu bản sao vào `CameraInitParam` (để "về mặc định"). `CameraSave.Save/Load` giữ yaw/pitch/zoom khi đổi cảnh cùng `scnType`.

Lua lấy/đặt: `JXM.GetCameraData()` trả 6 số từ `ScnUnit.mScnExcel.cameraInit` (mặc định 10 · 0,5 · 10 · 90 · 30 · 1 khi thiếu) ; `JXM.TweenCamera(0, Dis,minDis,maxDis,AngleX,AngleY,Hoffset,time,bMayControl)` / `JXM.TweenCamera(1, minDis,maxDis,Hoffset,time,bMayControl)` → `GameControl.TweenCamera` → tác vụ `TweenCamera` (`Mathf.Lerp` từng tham số theo `mCurTime/mTime`, xong thì đặt lại `bMayControl*`).

### 3.4 Vùng cảnh đổi camera (`scn_area.lua` + bảng `scn_area`)

Cột `camera` = "距离*最小距离*最大距离*X角度*Y角度*高度偏移*相机转动时间" (7 số) và `cameracontrol` (1 = cấm người chơi xoay). Vào vùng: `IntoChgCamera` → `JXM.TweenCamera(0, …)`; ra vùng: về vùng cũ hoặc `JXM.TweenCamera(1, scn.minDis, scn.maxDis, scn.HeightOffset, 0.2, …)` (hồi 0,2 s). **Bản này chưa vùng nào điền camera** (đã quét `scn_area_list`). Ngoài ra còn `CameraModifyNode` (trigger collider, `LerpAngle/Lerp` với `mSpeed` + `mSpeedStep` tăng dần, `mInAreaMayCtrlAngle/Wheel`) — có mã nhưng không có trong bundle.

### 3.5 Camera kịch bản / kỹ năng / rung

- Sự kiện kỹ năng (`mbtb_skill_event`): `SkillEventRegister.SetCameraAnim / PlayCameraAnim / SetCameraBind` → `EventCamera.SetCamAnim(param, entity)` / `PlayAnim(name, startTime)` (Animator gắn trên MainCamera, thiếu thì log `PlayAnim err.MainCamera not find Animator`) / `BindTarget(creature)`; `CameraAnim` (ITask): `LerpAngle` yaw/pitch, `Lerp` bán kính/HeightOffset trong `fUpdateTime`, giữ `fAllTime`, rồi quay về (`fBackTime`, `bReset`, khôi phục `mOrigFollow`).
- Cốt truyện: `StoryAnimRegister.PlayCameraAnim`; Lua `talk_func.lua`: `Camera.SetGameCameraTarget(obj)`, `Camera.PlayGameCameraAnim(name)`; `CameraTargetMotifier` (đổi vị trí/góc trong `mTime`, `mBack/mBackTime`).
- Rung: `SetShake(time, period, swing)` → mỗi `period` giây cộng `Random.insideUnitSphere*swing` vào `localPosition`, hết thì trả về.
- Mờ: `CameraBlur.StartBlur(offsetBegin, offsetEnd, duration)` (Lua `StartGameCameraBlur/EndGameCameraBlur`), `CameraFade` (phủ màu OnGUI).

## 4. Điều khiển (chạm / chuột / tay cầm) — `EventCamera.TouchControl`

`EventCamera.Start` tạo `TouchControl(this)` rồi `InputMgr.RegisterTouch` + `RegisterMove`. `InputMgr.Update` → `UpdateJoystick` (trục Unity) + `UpdateMouseEvents` (PC) + `UpdateTouchEvents` (Android/iOS) → phát `TC_TouchBegin/Move/End(index, pos)` và `TCJoystick_*`.

| Sự kiện | Điều kiện | Kết quả |
|---|---|---|
| `TC_TouchBegin` | camera bật, `bMayControlAngle`; **`UICamera.Raycast(pos)` trúng UI → bỏ**; PC (`touchCount==0`) chỉ nhận `index==1` = **chuột phải** | ngón 1: ghi `mLastPos`, `mTouchIndex`; ngón 2 (nếu `bMayControlWheel`): `mLastPos2`, `mTouchIndex2` |
| `TC_TouchMove` 1 ngón | `|dx|>1 ∨ |dy|>1` px | `mLastPosDt = (delta + mLastPosDt) * 0.2` (lọc mượt) → `OnRotate(mLastPosDt.x, mLastPosDt.y)` |
| `TC_TouchMove` 2 ngón | ngón di > 5 px | `OnWheelChg((newDist − oldDist) × −0.01)` (pinch: xa nhau = lại gần) |
| `TC_TouchEnd` | ngón 1 nhả | `OnTouchEnd()`: `bTouch=false`, `fTouchWaitTime=1`, `bFollowSmooth=true`; nếu còn ngón 2 thì ngón 2 thành ngón 1 |
| `TCJoystick_Rotate(x,y)` | trục `HorizontalView/VerticalView` = **cần phải tay cầm** (axis 3/4, Vertical đảo) | `OnRotate` |
| `TCJoystick_WheelChg(dt)` | trục `ScrollWheel` (con lăn chuột đảo dấu; tay cầm axis 2 ×0,2), bị chặn khi có bảng UI đang mở | `OnWheelChg(dt)` |
| `OnRotate(x,y)` | | `mAngleX += x·angleSpeedRate; mAngleY −= y·angleSpeedRate; mAngleY = clamp(min,max); bTouch=true; GameControl.SertCameraAutoFollowAngle(false,1,false)` (báo Lua `eLuaEvent_OnCameraAutoFollow`) |
| `OnWheelChg(dt)` | | `mWheelDistance += dt·fWheelSpeed` |

Di chuyển tương đối camera: `InputMgr.UpdateJoystick` lấy `Horizontal/Vertical` (WASD/mũi tên, tay cầm) → góc `atan2`·57,2958 → `TCJoystick_Move(fAngle)` → `GameCamera.OnAngleMove`: `worldAngle = fmod(fAngle + camera.eulerAngles.y (+360 nếu âm), 360)` → `GameControl.OnAngleMove` → `SearchUnit.WasdMove` / `TaskKinematicMove.SyncKinematicMove` (gửi lại khi lệch > 2°, `Mathf.DeltaAngle`); nhả → `OnMoveEnd` → `SearchUnit.StopMove`. Hàm ngược `WorldAngletoScreen`. Kéo kỹ năng: `ui_primary_rocker.lua`: `SkillDragAngle = gócCần + Game.Camera.GetGameCameraAngle()`.

Khoá/GM: nút `md_btn_camera` (`ui_primary_high.lua`) lật `JXM.lockCamera`; GM `Game.Camera.CameraLock(b)`, `UseFreeCamera(b)` (`FreeCamera`: bay tự do bằng phím, `MoveSpeed`, `mTrunSpeed`, `AngleSpped`), chỉnh `freeCameraMoveSpeed/TurnSpeed` trong `ui_gm_easymode.lua`.

## 5. Bảng lớp/hàm camera (để tra tiếp)

`GameCamera` (RVA PC: `UpdateCameraParam 0x4ae1d0`, `OnUpdate 0x4ae9b0`, `OnRotate 0x4af2d0`, `OnWheelChg 0x4af3d0`, `OnAngleMove 0x4af090`, `get_targetPos 0x4ad5e0`, `OnStart 0x4adee0`, `FollowLookAt 0x4add20`, `TowardTarget 0x4ade40`), `EventCamera` (`InternalUpdate 0x4abb20`, `Awake 0x4a9b80`, `Start 0x4a9f20`, `SetShake 0x4ab430`, `PlayAnim 0x4ab4b0`), `EventCamera/TouchControl` (`TC_TouchBegin 0x4abf70`, `TC_TouchMove 0x4ac120`, `TC_TouchEnd 0x4ac3c0`, `TCJoystick_WheelChg 0x4ac510`), `FreeCamera`, `TweenCamera.RenderTick 0x553df0`, `CameraAnim.RenderTick 0x520ab0`, `CameraModifyNode`, `CameraInitParam`, `CameraSave`, `CameraBuildingFade.Update 0x4a3ee0`, `CameraTargetMotifier`, `InputMgr` (`UpdateJoystick 0x4b6520`, `UpdateMouseEvents 0x4b5870`), `GameControl` (`Init 0x47f850`, `TweenCamera 0x483a60/0x483dd0`, `OnAngleMove 0x481770`), `LuaReg_Camera` (`SetGameCameraTarget, PlayGameCameraAnim, GameCameraTowardTarget, StartGameCameraBlur, EndGameCameraBlur, CameraLock, UseFreeCamera, GetGameCameraAngle, Set/Get_freeCameraMoveSpeed/TurnSpeed`), `LuaReg_JXM` (`TweenCamera, GetCameraData, Set/Get_LockCamera, SetModelViewCameraNearClipPlane`). Toàn bộ dịch ngược có chú thích: `D:\game3gTQ_mo\cam_disasm.txt`, `cam_disasm2.txt`, `gc_cam.txt`, `scene_init.txt`; định nghĩa lớp: `cam_classes_res.txt`; toàn bộ 1 772 lớp: `meta_pc_res.txt`.

## 6. Cái gì mang được sang JX1 mobile (chỉ là gợi ý, chưa làm)

- **Không phải phép quay** (JX1 là sprite 8 hướng + nền phẳng vẽ sẵn). Nếu chủ vẫn muốn "nhìn rộng ra" thì hướng khả thi là **pan/lia nền** như ghi chép cũ, không phải xoay.
- **Cách chạm**: kéo một ngón trên vùng trống (UI ăn trước qua raycast) = lia; hai ngón = zoom kiểu `(khoảng cách mới − cũ) × hệ số`; lọc mượt `dt = (delta + dt_cũ) × 0,2`; ngưỡng 1 px để bỏ rung tay, 5 px cho pinch.
- **Tự về sau khi thả tay**: chờ 1 s rồi trôi về vị trí mặc định 40 đơn vị/s bằng `MoveTowardsAngle`-kiểu (với JX1 là trôi vị trí lia về nhân vật) — tạo cảm giác "camera tự biết về".
- **Đọc tham số theo cảnh** (`cameraInit` 7 số trong bảng cảnh, vùng cảnh có thể ghi đè + thời gian tween 0,2 s) — JX1 có thể đặt mức zoom/pan mặc định theo map trong `settings`.
- **Cần trái theo hướng camera**: không cần cho JX1 vì không xoay.
- **Làm mờ vật che** (`CameraBuildingFade`): JX1 đã có cơ chế mờ mái nhà riêng của Represent3, không cần.

## 7. Cách họ giấu dữ liệu và cách mở (đã làm, chạy lại vô hại)

1. Bundle `.bdd` = UnityFS mã hoá Unity-China (`AssetBundle.SetAssetBundleDecryptKey(key)` gọi trong `WBase.AssetMgr/InitAssetsContext.Load`).
2. Khoá 16 ký tự chữ-số nằm **mã hoá** trong `resources.assets` → MonoBehaviour `GlobalInfos` (`CryptAB=True`, `CrcAB=True`, `rk=<base64 16 byte>` + 4 bundle có CRC).
3. `Game.Utils.AbBoot.TryUnseal`: `key16 = SHA256( (_w0 ^ 0x5A) ‖ (_w1 ^ 0x4B) )[:16]` với `_w0 = f29c417a13e8550b`, `_w1 = 6dc32891a45e17df` (hằng `long` trong `<PrivateImplementationDetails>`, đọc từ giá trị mặc định trường trong metadata), rồi `AES-128-ECB` không padding giải `rk` → kiểm chuỗi chữ-số.
4. Kết quả: **PC `MLL8Fps15BNaFvsh`**, **Android `DcmWEdJRL448nU6m`** (cùng `_w0/_w1`, khác `rk`). `UnityPy.set_assetbundle_decrypt_key(...)` là đọc được hết.

## 8. Bộ công cụ trong `D:\game3gTQ_mo` (Python thuần, không tải công cụ ngoài; đã `pip install UnityPy lz4 pycryptodome`)

| Tệp | Việc |
|---|---|
| `il2cpp_meta.py` | đọc `global-metadata.dat` v31 → `meta_pc.txt`/`meta_apk.txt` (lớp, trường, hàm, chỉ số kiểu) |
| `il2cpp_bin2.py` | tìm `CodeRegistration`/`MetadataRegistration` trong `GameAssembly.dll`, dịch chỉ số kiểu → tên, gán **RVA** cho 55 809 hàm → `meta_pc_res.txt` |
| `disasm.py "Lop.Ham"` / `rva:0x…` | dịch ngược x64 (capstone) có chú thích: tên hàm gọi, `?Lop.truong` cho `[reg+off]`, chuỗi/kiểu/trường qua metadata-usage, hằng float, chuỗi icall |
| `loc.sh 'Lop.Ham()'` (`DIS=tệp`) | in một hàm đã dịch, bỏ nhiễu khởi tạo lớp |
| `goi_ai.py Lop.Ham…` | ai gọi hàm (quét `call/jmp rel32`) |
| `ai_dung_chuoi.py "chuỗi"` | hàm nào dùng chuỗi literal |
| `lay_khoa.py`, `lay_khoa_apk.py` | tính khoá bundle PC/Android → `khoa_bundle.txt`, `khoa_bundle_apk.txt` |
| `quet_bundle2.py`, `quet_tatca.sh` | quét 68 bundle → `bundle_idx/*.tsv`, `bundle_all.tsv` (207 319 đối tượng), rút TextAsset → `pc_textassets/` (653 tệp) |
| `doc_camera_bundle.py` | 53 Camera trong bundle + script cùng GameObject → `camera_bundle.txt`, `camera_fov.txt` |
| `chuoi.py` | rút chuỗi ASCII từ nhị phân → `chuoi_*.txt` |

Bẫy đã gặp: heredoc Bash nuốt `\` và `'` → viết tệp Python bằng công cụ Write; console cp1252 sập với tên Trung Quốc → `PYTHONIOENCODING=utf-8`; UnityPy đọc MonoBehaviour thiếu typetree → `read(check_read=False)`; `codeGenModules` xếp theo tên nên `__Generated` (không đuôi `.dll`) đứng giữa; thứ tự `MetadataRegistration` bản này: `types(6,7) · metadataUsages(8,9) · fieldOffsets(10,11) · typeDefinitionsSizes(12,13)`; metadata-usage v27+ = `(kind<<29 | idx<<1 | 1)` nằm sẵn trong `.data`.


---

## 9. Mổ thêm 14/09 (chủ hỏi): hiệu ứng trang bị theo màu khi mặc, và hình nhân vật ở màn tạo nhân vật

Nguồn: `meta_pc_res.txt` (lớp/trường), `bundle_all.tsv` (68 bundle PC, đủ toàn bộ `StreamingAssets`), bảng dữ liệu rút bằng UnityPy (utf-16, ở `scratchpad/ta_*`), Lua `ui_choose.lua` (màn chọn/tạo nhân vật). Chỉ phân tích, chưa làm gì cho JX1.

### 9.1 Trang bị mặc trên người: hiệu ứng theo phẩm chất nằm ở VŨ KHÍ, không ở thân

| Cơ chế | Bằng chứng |
|---|---|
| **Đao quang (vệt sáng khi vung vũ khí) đổi theo phẩm chất** | bảng `anim_effect` 18 hàng: hàng 1–6 = "通用白色/蓝色/紫色/金色/白金/玄金武器" (vũ khí trắng / lam / tím / kim / bạch kim / huyền kim): mỗi hoạt ảnh đánh (`gj01`, `gj02`, `gjdj01`…) trỏ một `sfx特效id` khác nhau (290…295). Bảng `model_hang_list` cột "武器拖尾动作特效 anim_effect (0 = mặc định **theo phẩm chất**)"; `cha_pic.defAnimEffect` = nhóm mặc định. → cùng một vũ khí, phẩm chất cao hơn thì vệt sáng màu khác. |
| Mô hình vũ khí riêng từng món | `model_hang_list` 96 hàng: tên, điểm treo (tay phải/trái), tài nguyên `Assets/StaticModels/weapons/<loại>/<tên>` (jian_tiebishou, jian_gang…), loại (kiếm/đao/thương/côn/song đao/song chuỳ/quyền/phi tiêu/phi đao/nỏ/ áo choàng), nhóm hoạt ảnh, kỹ năng kèm. Không hàng nào dùng `mh_xweapon_sfxobj` (hiệu ứng gắn cố định) → sáng theo màu là do đao quang, không phải cột sáng gắn trên vũ khí. |
| Chất liệu vũ khí theo cấp | texture `weapon_diji_01 / zhongji_01 / gaoji_01` (thấp / trung / cao cấp), material `weapon_rimcolor` (viền sáng rim) — màu rim lấy từ `Define.ItemColEffectWhite/Blue/Purple/Gold/WGold/XGold` (6 màu phẩm chất, cũng dùng cho viền ô vật phẩm `itemborder_anim_path` và cột sáng đồ rơi). |
| Thân người | không có hào quang theo phẩm chất. Áo/mũ/giày chỉ đổi **da** (`cha_pic` "皮肤 身体*头*鞋", `model_list` 63 hàng phần lớn là thú cưỡi có hiệu ứng: "帅气马特效闪电/黄绿/火焰"…). Bộ trang bị (`equip_suit`, 6 bộ) chỉ cộng thuộc tính, không có hiệu ứng hình. |
| Màu thân khi trúng trạng thái | `state_effect` 77 hàng: trúng độc nhuộm xanh `32A260`, bỏng đỏ `B30D0D`, chậm vàng `CCCC44`, kèm đổi cỡ / trong suốt / tốc độ hoạt ảnh. |

**Mang sang JX1 được gì (đề xuất, chưa làm):**
1. **Hào quang dưới chân theo phẩm chất trang bị cao nhất đang mặc** — JX1 đã có sẵn cơ chế và ảnh: `spr/haoquang/vongtrondo / vongtrontim / vongtronvang / vongtronxanh.spr` (vòng 200×200, 30 khung, neo 100,125) đang dùng cho vòng hào quang người chơi. Việc: chọn màu theo `m_nColorID` cao nhất của vũ khí/áo đang mặc, vẽ trước sprite nhân vật (mobile-only trong `KPlayer`/`KNpc` draw). Khoảng nửa ngày.
2. **Đao quang màu khi vung vũ khí** (đúng cái game 3D làm): JX1 vung vũ khí là sprite 8 hướng vẽ liền vũ khí, không có lớp vệt sáng riêng → phải vẽ thêm sprite vệt sáng cho từng loại vũ khí × 8 hướng × vài khung, rồi nhuộm màu bằng `COLOR_ADJUST` như cột sáng đồ rơi. Cần vẽ ảnh mới (không có sẵn), 2–3 ngày, hiệu quả đẹp nhưng tốn công vẽ.
3. Viền sáng vũ khí kiểu rim: không làm được trên sprite 2D.

### 9.2 Hình nhân vật ở màn tạo nhân vật: là MÔ HÌNH 3D sống, không phải ảnh

- `ui_choose.lua`: mỗi nghề (`mb_profession[cid].show_chapic`) tạo một sinh vật UI `JXM.GO_CreateUICreature(chalist, chapic, true)` đặt vào `ModelView` (camera UI riêng, bóng đổ ở (0,3000,0)), bấm chọn thì `SelectCreateActor`. `cha_list` 1–5 = "初始金/木/水/火/土系职业" (5 hệ ngũ hành). `cha_pic` 3–10 = nhân vật **trình diễn** ("展示-金系男, 木系男, 木系女, 水系女, 火系男, 火系女, 土系男, 土系女") với bộ xương `xr01_01…xr05_02`, chân dung `zj_011/021/031/041/181`; 1–2 = "标准男/女" xương `zj01/zj02` (nhân vật thật trong game).
- Tài nguyên trong 68 bundle PC: **lưới da** có đủ: bundle `d4b79c319044.bdd` (11 MB, 278 Mesh): 23 thân `skin@zj010…zj023` (23 bộ trang phục), đầu `skin@tou01/02@…`, giày `skin@xie01/02@…`, cùng da NPC/thú (yezhu, yemao, xiong…). Đã thử **xuất được OBJ** bằng UnityPy (`skin_zj010.obj` 330 KB, 25 bind pose = có xương). **Hoạt ảnh** có đủ theo `animation_list` 60 mục (`xx01` nghỉ, `zp01` chạy, `gj01/02` đánh, `sf01` bị đánh, `jump01`…, thêm `qm_*` cưỡi ngựa, `skill_*`), nhóm theo vũ khí (`anim_group` 24: tay không/kiếm/đao/thương/côn/song đao…). **Chưa thấy** prefab bộ xương `zj01/xr0x` và texture da theo tên trong bảng (có `skin@001…abc`); chân dung `zj_011…` nằm trong atlas NGUI (phải đọc rect từ `UIAtlas`), chưa rút.
- Vậy "lấy nhân vật lúc tạo game" = lấy mô hình 3D có xương + 60 hoạt ảnh + da. JX1 mobile là engine 2D sprite, **không vẽ được mô hình 3D**; muốn dùng phải **vẽ lại thành sprite 8 hướng** (Blender: ghép xương, gắn hoạt ảnh, render 8 hướng × từng động tác JX1 cần: đứng/đi/chạy/đánh theo loại vũ khí/ngồi/chết… × 5 hệ × 2 giới) — quy mô dựng lại toàn bộ nhân vật, tính bằng tuần, và sẽ **lệch phong cách** với NPC/quái JX1 hiện có.
- Cách dùng rẻ và hợp lý nếu chủ muốn: chỉ dùng cho **màn tạo nhân vật** — render tĩnh (hoặc xoay vòng vài khung) 8 nhân vật trình diễn thành PNG → SPR làm hình đại diện 5 hệ trong màn tạo nhân vật JX1 mobile. Việc: rút mesh + tìm texture/xương (1 ngày, có thể phải soi bundle APK), Blender render (1–2 ngày), ghép vào UI JX1 (1 ngày).
- **Lưu ý bản quyền:** toàn bộ mô hình, hoạt ảnh, ảnh này thuộc nhà phát hành Kiếm Võng Giang Hồ. Đưa vào sản phẩm phát hành cần quyền của họ; đây là quyết định của chủ, tôi chỉ nêu để cân nhắc.


---

## 10. Mổ sâu (chủ hỏi 14/09): đao quang, hào quang, và hiệu ứng trang bị trên trang thông tin nhân vật

Nguồn: bảng `sfx_object` (127 hàng), `anim_effect` (18), `sfx_ui_object` (7), `cha_pic`, `cha_list`, `fabao_list` (80), `xianjie_list` (15), `title_list` (62), `avatar_list` (75) rút từ bundle `c96f69a275af.bdd`; Lua `lua_itemMgr.lua`, `ui_propequip.lua`, `ui_lookequip.lua`, `ui_choose.lua`; lớp C# `SFXXWeaponAnim/Anchor/Adapter`, `SFXMeshTrailDrag`, `XftWeapon.XWeaponTrail`, `AnimateBagSprite`; bundle hạt `1d472c44c423.bdd` (1 455 prefab, 315 ParticleSystem, 93 texture). Bảng đã rút ở `scratchpad/ta_*.txt`, chép về `D:\game3gTQ_mo\pc_textassets\`.

### 10.1 Đao quang (vệt sáng vũ khí) — chuỗi đầy đủ

1. **Chọn nhóm:** `Player.m_weaponAnimEffect` → hàng `anim_effect`. Vũ khí thường: `model_hang_list` cột "武器拖尾动作特效 = 0 → theo **phẩm chất**", nên phẩm chất `eItemQuality` 1 trắng / 2 lam / 3 tím / 4 kim / 5 bạch kim / 6 huyền kim ↔ `anim_effect` 1…6 ("通用白色…玄金武器"). NPC dùng `cha_pic.defAnimEffect` (nhóm 10–14 "相加" cộng sáng: gợn sóng / sụp / chấm / mềm; NPC đặc biệt 100–101). Nhân vật trình diễn ở màn tạo dùng 102–105 ("炫光" chói: kim, hỏa, hỏa-kim, độc).
2. **Mỗi hoạt ảnh đánh một sfx:** hàng nhóm liệt kê `gj01/gj02` (đánh thường), `gjdj01/02` (đao kiếm), `gjqg01/02` (thương côn), `gjss01/02` (song thủ), `gjyc01/02` (đánh xa), `qm_gj01/02` (trên ngựa) → cùng một `sfx_object` id: **290 → 295** = `Particles/Daoguang/dg_xw_cmn_{white, blue, purple, gold, wgold, xgold}` (prefab có trong bundle hạt). NPC: 305–308 `dg_xw_add_{bowen, taxian, xiantiaodian, rouguang}`; trình diễn: 309–311 `dg_xw_emis_{huoyan, gold, huojin}`, 507–508 `dg_wx_xr_muxi` gắn xương `npc_wqdrag_l_1 / r_1` (vệt hai tay, 45 khung).
3. **Cách vẽ:** `SFXXWeaponAnim` trên `Creature.mWeaponAnimFx`: bảng `XWeaponAnimData{name, max_frame, hinge_start_1/end_1, hinge_start_2/end_2, frame_ctrl, sound_id}` = vệt chỉ sống trong khoảng khung [start, end] của hoạt ảnh (hai vệt cho song thủ); vệt là `XftWeapon.XWeaponTrail` (trail1/trail2) căng giữa `SFXXWeaponAnchor.startPoint/endPoint` (chuôi → mũi vũ khí), màu `mColor`; loại mesh dùng `SFXMeshTrailDrag` (màu, độ sáng `mEnhance`, đường cong dài/ngắn/cỡ, cuộn UV, alpha sinh/tắt). `SFXXWeaponAdapter` co dãn hạt theo chiều dài vũ khí (`scaleZ`, `scalePctLife/Speed`, `adapterColor`).
4. **"Phụ ma" (附魔, hào quang bám dọc lưỡi vũ khí):** `SFXXWeaponAnchor.mModelFuMoID / mFuMoNode / mFuMoFX` + `rimTrans` — hiệu ứng thường trực trên vũ khí, đến từ hệ **pháp bảo** (`fabao_list` cột "附魔百分比 / 每级附魔值 / 附魔道具"; 80 pháp bảo, 3 phẩm mỗi cái, mỗi phẩm một ngoại hình `weapon_list`), không phải từ phẩm chất.
5. **Texture nguồn (2D, rút được):** `m_daoguang11`, `m_daoguang11_fun`, `m_daoguang_05`, `sprite_dg_add_512`, `sprite_dg_add_512_2`, `sprite_dg_meshtrail_mul_512`, `trail_particle_128_01…05`, `trail_shockwave_01…04` (bundle hạt). Đã xuất thử PNG: `scratchpad/tex_m_daoguang11.png`… (`daoguang_xemtruoc.png`).

**Mang sang JX1:** JX1 vẽ vũ khí liền trong sprite nhân vật, không có xương chuôi/mũi → không căng vệt theo vũ khí được. Cách gần nhất: **sprite vệt sáng riêng** cho từng động tác đánh (mỗi loại vũ khí × 8 hướng × 3–4 khung), vẽ chồng lên nhân vật trong khung [start, end] của hoạt ảnh đánh (JX1 có số khung từng động tác trong bảng sprite), **nhuộm màu theo phẩm chất** bằng `COLOR_ADJUST` như cột sáng đồ rơi (một bộ ảnh trắng, 6 màu). Ảnh có thể vẽ tay từ texture `m_daoguang*` làm mẫu. Ước: 2–3 ngày (1 ngày ảnh cho kiếm/đao, 1 ngày mã `KPlayer`/`KNpc` mobile-only, nửa ngày các loại vũ khí còn lại). Không có texture nào lấy dùng thẳng được vì vệt của họ là dải kéo theo chuyển động 3D.

### 10.2 Hào quang — trong game 3D là 6 thứ khác nhau

| Loại | Dữ liệu | Ghi chú |
|---|---|---|
| **Vòng dưới chân NPC theo cấp** | `sfx_object` 1000 `Halo/halo_npc_purple` (quái tím/tinh anh), 1001 `halo_npc_gold` (quái hoàng kim), 1002 `halo_npc_pink`, 1003 `halo_boss_red` (pháp trận đỏ boss), 1005 `State/state_boss_gold_01`; gắn `sys_foot`, sống -1 (thường trực) | prefab có trong bundle hạt; cách gần JX1 nhất |
| Hào quang kỹ năng phái Nga My | 925–928 `Halo/halo_em_{mengdie, foxinciyou, qingyin, puduzs}` (chân / thân) | buff kỹ năng |
| Chói ở màn tạo nhân vật | `anim_effect` 102–105 → `dg_xw_emis_*`, `dg_wx_xr_muxi` cho `cha_pic` 3, 4, 6, 7, 8 (kim nam, mộc nam, thuỷ nữ, hoả nam, hoả nữ) trong hoạt ảnh `xrzs` (trình diễn) | đây là cái "hào quang" thấy lúc tạo nhân vật |
| Thời trang | `avatar_list` loại 1: 8 bộ × mỗi phái/giới (皇帝的新衣, 飞龙在天, 金榜题名…), đổi da `skin@zj010…023` | không có ánh sáng riêng |
| Tường vân (mây cưỡi bay) | `avatar_list` loại 2 (紫电黑云, 五彩祥云, 火羽霞云) đi với **tiên giai** `xianjie_list` 15 bậc (居士→…) có cột "飞行高度" (độ cao bay) | thú cưỡi bay, JX1 không có |
| Pháp bảo, danh hiệu | `fabao_list` 80 (3 phẩm, phụ ma); `title_list` 62: màu chữ `[c=…]`, ảnh danh hiệu `tl_spr`, prefab `title_gx/title_gx01` (danh hiệu có ánh sáng) | JX1 có danh hiệu chữ |
| Nhuộm thân theo trạng thái | `state_effect` 77: độc `32A260`, bỏng `B30D0D`, chậm `CCCC44`, đổi cỡ/alpha | JX1 có đổi màu sprite khi trúng độc |

**Mang sang JX1 (đề xuất):** (1) **vòng sáng dưới chân quái tinh anh / boss** theo màu (tím, vàng, đỏ) bằng `spr/haoquang/vongtron{tim,vang,do}.spr` có sẵn, vẽ trước sprite NPC, chọn theo cấp/loại NPC — nửa ngày; (2) vòng hào quang người chơi theo phẩm chất trang bị cao nhất — nửa ngày (đã nêu §9); (3) hào quang danh hiệu: JX1 có danh hiệu chữ, thêm vòng/ảnh nhỏ theo danh hiệu — 1 ngày, cần ảnh.

### 10.3 Trang thông tin / trang bị của nhân vật — hiệu ứng nằm trên Ô TRANG BỊ, không có mô hình

- `roleinfo` prefab chỉ có `name, lv, bg`; `ui_propequip` (trang thuộc tính + trang bị) và `ui_lookequip` (xem đồ người khác) **không dùng ModelView** (ModelView chỉ ở màn chọn/tạo nhân vật `ui_choose`). Nên "hiệu ứng trang bị mặc trên trang thông tin" = hiệu ứng trên **ô icon**:
  1. **Viền phẩm chất động** (走边动画): `lua_item.SetBorderType(goAnim, quality)` bật con thứ `QualityToBorderID`: lam 0, tím 1, kim 2, bạch kim 3, huyền kim 4 (trắng không viền) — prefab viền chạy (`itemborder_anim_path` của `Global`), dùng chung ở túi, tips, trang bị.
  2. **Ánh quét theo cấp cường hoá** (走光): `GetLevelFlash(item)` → `AnimateBagSprite.Play(x, y, ex, ey, prefix, fps, thickness)` với bảng `EquipLevelUpFlash`: +6…+8 `jialiu_` (2–3 fps, mỏng), +9…+11 `jiajiu_` (5–7 fps, dày 4–6), +12…+14 `xuanhong_` / `anjin_` (đỏ sẫm / vàng sẫm, 7–9 fps, dày 25; ngũ hành "kinh diễm" dùng bộ thứ hai), +15 `cai_` (nhiều màu, 8 fps, dày 20) — là các dãy ảnh trong atlas UI chạy vòng quanh icon.
  3. **Màu chữ +N** theo cấp: +1–5 trắng, +6–8 xanh nhạt `82e4ff`, +9 vàng `ffcb16`, +10 cam `ff7f2a`, +11 hồng `ff5ab3`, +12 đỏ `ff2323`, +30 vàng viền đỏ, +50 vàng viền tím.
  4. **Hạt trong UI:** `ui_propactive.prefab` khi ô được kích hoạt; `sfx_ui_object`: chuẩn bị/thành công thăng cấp trang bị, truyền thừa, chọn đá quý (trắng/tím), thăng kỹ năng, đặt lại tứ cách — mỗi cái một prefab `Particles/UI/*`, sống 12–55 khung.
  5. Tips trang bị: màu tên theo phẩm chất `Define.ItemColEffect*`, bộ (`equip_suit`) chỉ là thuộc tính.

**Mang sang JX1 mobile:** bảng trang bị/hành trang của JX1 vẽ icon qua `KWndImage`/`DrawPrimitives`; thêm (1) **viền phẩm chất động** = một SPR viền 4–6 khung màu trắng nhuộm theo `m_nColorID` (xanh/hoàng kim/tím/bạch kim), vẽ chồng lên ô trang bị đang mặc và ô túi — 1 ngày (ảnh viền sinh bằng PIL như cột sáng); (2) **ánh quét** theo cấp cường hoá — JX1 có cường hoá "+N" không thì phải xem bảng vật phẩm; nếu có, thêm nửa ngày. Mô hình nhân vật 3D trên trang thông tin: game 3D cũng không làm, JX1 giữ chân dung như hiện tại.
