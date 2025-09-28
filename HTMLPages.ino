// HTMLPages.ino


// ─── portal Page Html ────────────────────────────────────────────────────
const char portalPageHtml[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>VIGL E-Paper</title>
    <style>
      body{margin:0;font-family:sans-serif;background:#f5f5f5}
      .container{max-width:480px;margin:40px auto;background:#fff;
        padding:24px;border-radius:8px;box-shadow:0 4px 12px rgba(0,0,0,0.1)}
      h1{text-align:center;color:#333;margin-bottom:24px}
      .btn-group{display:flex;justify-content:center;gap:12px;flex-wrap:wrap;margin-bottom:24px}
      button{background:#98b261;color:#fff;border:none;
        padding:12px 16px;border-radius:4px;cursor:pointer;transition:background .3s}
      button:hover{background:#738844}
      hr{border:none;border-top:1px solid #eee;margin:24px 0}
      .status p, .info p{margin:6px 0;color:#333}
      .preview{margin-top:16px;text-align:center}
      .image-container{position:relative;width:240px;height:360px;margin:auto}
      .loader{border:3px solid #f3f3f3;border-top:3px solid #98b261;
        border-radius:50%;width:16px;height:16px;animation:spin 1s linear infinite;
        display:inline-block;vertical-align:middle;margin-right:6px}
      @keyframes spin{0%{transform:rotate(0);}100%{transform:rotate(360deg);}}
    </style>
  </head>
  <body>
    <div class="container">
      <h1>VIGL E-Paper</h1>
      <div class="btn-group">
        <button onclick="location.href='/config'">Screen Settings</button>
        <button onclick="location.href='/upload'">Upload Image</button>
      </div>
      <hr>
      <div class="status">
        <p>Hotspot Status: <span id="apStatus"><span class="loader"></span></span></p>
        <p>Station Wi-Fi: <span id="staSsid"><span class="loader"></span></span>
          @ <span id="staIp"><span class="loader"></span></span></p>
        <p id="apInfoLine">AP Wi-Fi: <span id="apSsid"><span class="loader"></span></span>
          / <span id="apPass"><span class="loader"></span></span>
          @ <span id="apIp"><span class="loader"></span></span></p>
      </div>
      <hr>
      <div class="preview">
        <h2>Current Displayed Image</h2>
        <div class="image-container">
          <div class="loader" id="imgLoader"></div>
          <img id="previewImg" alt="Displayed image" style="display:none;width:100%;height:100%;object-fit:contain;transform-origin:center center;">
        </div>
        <div class="preview-actions" style="margin-top:12px;">
          <button id="displayCurrentBtn">Display Current Image</button>
          <button id="clearImageBtn">Clear Image</button>
          <button id="rotateBtn">Rotate Image</button>
          <button id="downloadBtn">Download PNG</button>
        </div>
      </div>
    </div>
    <script>
      let rotated = true;
      function fetchStatus() {
        fetch('/status').then(r=>r.json()).then(data=>{
          document.getElementById('apStatus').textContent = data.apEnabled?'ON':'OFF';
          document.getElementById('staSsid').textContent = data.staSsid||'–––';
          document.getElementById('staIp').textContent   = data.staIp  ||'–––';
          document.getElementById('apSsid').textContent  = data.apSsid;
          document.getElementById('apPass').textContent  = data.apPass;
          document.getElementById('apIp').textContent    = data.apIp;
          document.getElementById('apInfoLine').style.display = data.apEnabled?'':'none';
        });
      }
      function loadImage() {
        let img = document.getElementById('previewImg'),
            loader = document.getElementById('imgLoader');
        loader.style.display = 'inline-block';
        img.style.display = 'none';
        img.onload = ()=>{ loader.style.display='none'; img.style.display='block'; };
        img.onerror = ()=>{ loader.style.display='none'; img.style.display='none'; };
        img.src = '/preview.bmp?'+Date.now();
        img.style.transform = rotated?'rotate(90deg)':'rotate(180deg)';
      }
      function toggleRotation(){
        let img = document.getElementById('previewImg');
        rotated = !rotated;
        img.style.transform = rotated?'rotate(90deg)':'rotate(180deg)';
      }
      function downloadImage(){
        const img    = document.getElementById('previewImg');
        const canvas = document.createElement('canvas');
        const ctx    = canvas.getContext('2d');

        const w = rotated ? img.naturalHeight : img.naturalWidth;
        const h = rotated ? img.naturalWidth  : img.naturalHeight;
        canvas.width  = w;
        canvas.height = h;

        ctx.translate(w/2, h/2);
        const angle = rotated ? -Math.PI/2 : Math.PI; 
        ctx.rotate(angle);

        ctx.drawImage(img, -img.naturalWidth/2, -img.naturalHeight/2);

        canvas.toBlob(blob => {
          const link = document.createElement('a');
          link.href     = URL.createObjectURL(blob);
          link.download = 'e-paper-image.png';
          link.click();
        }, 'image/png');
      }

      document.getElementById('displayCurrentBtn')
        .addEventListener('click',()=>{
          fetch('/display_current',{method:'POST'})
            .then(()=>setTimeout(loadImage,500));
        });
      document.getElementById('clearImageBtn')
        .addEventListener('click',()=>{
          fetch('/clear_image',{method:'POST'})
            .then(()=>{
              let img = document.getElementById('previewImg'),
                  loader = document.getElementById('imgLoader');
              img.src = '';
              img.style.display = 'none';
              loader.style.display = 'none';
            });
        });
      document.getElementById('rotateBtn')
        .addEventListener('click',toggleRotation);
      document.getElementById('downloadBtn')
        .addEventListener('click',downloadImage);
      window.addEventListener('load',()=>{ fetchStatus(); loadImage(); });
    </script>
  </body>
  </html>
)rawliteral";

// ─── Screen Settings Html ────────────────────────────────────────────────
const char screenSettingsHtml[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Screen Settings</title>
    <style>
      body{margin:0;font-family:sans-serif;background:#f5f5f5}
      .container{max-width:480px;margin:60px auto;background:#fff;
        padding:24px;border-radius:8px;box-shadow:0 4px 12px rgba(0,0,0,0.1)}
      .header{display:flex;align-items:center;justify-content:space-between;margin-bottom:24px}
      .header h1{margin:0;color:#333;font-size:24px}
      .home-btn{background:transparent;color:#98b261;border:none;padding:0;font-size:14px;cursor:pointer;transition:color .3s}
      .home-btn:hover{color:#738844;text-decoration:underline}
      form{display:flex;flex-direction:column;gap:12px;}
      input{padding:8px;font-size:16px;border:1px solid #ccc;border-radius:4px;}
      button.submit-btn,button.action-btn{background:#98b261;color:#fff;border:none;
        padding:12px;font-size:16px;border-radius:4px;cursor:pointer;transition:background .3s}
      button.submit-btn:hover,button.action-btn:hover{background:#738844;}
      .actions{display:flex;justify-content:space-around;margin-top:24px;flex-wrap:wrap;gap:12px}
    </style>
  </head>
  <body>
    <div class="container">
      <div class="header">
        <button class="home-btn" type="button" onclick="location.href='/'">← Home</button>
        <h1>Screen Settings</h1>
        <div style="width:50px"></div>
      </div>

      <form method="POST" action="/config">
        <input type="text" name="ssid" placeholder="SSID" required>
        <input type="password" name="password" placeholder="Password" required>
        <button class="submit-btn" type="submit">Save & Reboot</button>
      </form>

      <div class="actions">
        <button class="action-btn" id="toggleHotspot">Toggle Hotspot</button>
        <button class="action-btn" id="updateBtn">Update</button>
        <button class="action-btn" id="factoryReset">Factory Reset</button>
      </div>
    </div>

    <script>
      document.getElementById('toggleHotspot').addEventListener('click', () => {
        fetch('/toggleAP')
          .then(r => r.text())
          .then(txt => alert('Hotspot: ' + txt));
      });

      document.getElementById('updateBtn').addEventListener('click', () => {
        location.href = '/update';
      });

      document.getElementById('factoryReset').addEventListener('click', () => {
        if (confirm('Confirm factory reset? This will erase stored Wi-Fi and images.')) {
          fetch('/factory_reset', { method: 'POST' })
            .then(r => r.text())
            .then(txt => alert(txt));
        }
      });
    </script>
  </body>
  </html>
)rawliteral";

// ─── Upload Page Html ────────────────────────────────────────────────────
const char uploadPageHtml[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>e-Paper Image Editor</title>
    <style>
      html, body {
        margin:0; padding:0; height:100%; overflow:hidden;
        font-family:sans-serif; background:#f5f5f5;
      }
      .container { display:flex; height:100%; }

      .sidebar {
        width:160px; background:#fff; border-right:1px solid #ddd;
        display:flex; flex-direction:column; padding:8px; gap:8px;
              height:100%;
        overflow-y:auto;
        -webkit-overflow-scrolling:touch;
      }
      .sidebar button,
      .sidebar label,
      .sidebar input[type=range] {
        font-size:14px;
      }
      .sidebar button {
        background:#98b261; color:#fff; border:none;
        padding:8px; border-radius:4px; cursor:pointer;
        transition:background .3s;
      }
      .sidebar button:hover { background:#738844; }
      hr {
        border: none;
        height: 1px;
        min-height: 1px;
        background-color: #ddd;
        margin: 12px 0;
        width: 100%;
      }
      .main {
        flex:1; display:flex; flex-direction:column;
        align-items:center; padding:16px; box-sizing:border-box;
        overflow-y:auto;
      }
      #importBtn { margin-bottom:12px; }

      #imgContainer {
        position:relative;
        min-width:240px; max-width:240px; min-height:360px; max-height:360px;
        border:1px solid #ccc; background:#e0e0e0;
        overflow:hidden; touch-action:none; -webkit-touch-action:none; -ms-touch-action:none;
        cursor:grab;
      }
      #imgContainer.landscape {
        min-width:360px; max-width:360px; min-height:240px; max-height:240px;
      }
      #imgContainer img {
        position:absolute; top:0; left:0;
        user-select:none; -webkit-user-drag:none;
        touch-action:none; transform-origin:top left;
      }
      #imgContainer img.dragging { cursor:grabbing; }

      #dstBox {
        margin-top:16px;
        width:240px; height:360px;
      }
      #dstBox canvas {
        width:100%; height:100%; border:1px solid #ccc;
      }
    </style>
  </head>
  <body>
    <div class="container">
  <div class="sidebar">
        <input type="file" id="fileInput" accept="image/png,image/jpeg" style="display:none">
              <button id="importBtn">Import Image</button>
        <hr>

        <label>Scale
          <input type="range" id="scaleSlider" step="1">
        </label>
        <label><input type="checkbox" id="invertCheckbox">Invert Colors</label>
        <hr>
        <button id="btnToggleOri">Landscape</button>
        <hr>

        <label>Threshold
          <input type="range" id="thresholdSlider" min="0" max="255" value="128">
        </label>
        <label>Red Threshold
          <input type="range" id="redThresholdSlider" min="0" max="255" value="128">
        </label>
        <button id="btnThreshMono">Threshold Mono</button>
        <button id="btnThreshColor">Threshold Color</button>
        <hr>

        <label>Brightness
          <input type="range" id="brightnessSlider" min="-100" max="100" value="0">
        </label>
        <label>Contrast
          <input type="range" id="contrastSlider" min="50" max="200" value="100">
        </label>
        <button id="btnDitherMono">Dither Mono</button>
        <button id="btnDitherColor">Dither Color</button>
        <hr>

        <button id="btnUpload">Upload to Display</button>
        <hr>
        <a class="button" href="/"><button>Home</button></a>
      </div>
      <div class="main">
        <div id="imgContainer"></div>
        <div id="dstBox"></div>
      </div>
    </div>

    <script>
      let srcImg = null, imgX = 0, imgY = 0, startX = 0, startY = 0;
      let dragging = false, scale = 1, isLandscape = false;
      const destW = 240, destH = 360;
      const palBW    = [[0,0,0],[255,255,255]];
      const palColor = [[0,0,0],[255,255,255],[127,0,0]];

      const pointers = new Map();
      let initialPinchDistance = 0, pinchStartScale = 1, pinchCenter = {x:0,y:0};

      const fileInput    = document.getElementById('fileInput');
      const importBtn    = document.getElementById('importBtn');
      const imgContainer = document.getElementById('imgContainer');
      const dstBox       = document.getElementById('dstBox');
      const btnToggleOri = document.getElementById('btnToggleOri');
      const scaleSlider  = document.getElementById('scaleSlider');

      function recalcScaleLimits() {
        const cw = imgContainer.clientWidth, ch = imgContainer.clientHeight;
        const minS = Math.max(cw/srcImg.naturalWidth, ch/srcImg.naturalHeight);
        const maxS = Math.min(10, srcImg.naturalWidth/cw, srcImg.naturalHeight/ch);
        scaleSlider.min = Math.round(minS*100);
        scaleSlider.max = Math.round(maxS*100);
        if (scale*100 < scaleSlider.min) scale = scaleSlider.min/100;
        if (scale*100 > scaleSlider.max) scale = scaleSlider.max/100;
        scaleSlider.value = Math.round(scale*100);
      }

      function clamp() {
        const cw = imgContainer.clientWidth, ch = imgContainer.clientHeight;
        const wS = srcImg.naturalWidth*scale, hS = srcImg.naturalHeight*scale;
        imgX = Math.min(0, Math.max(cw - wS, imgX));
        imgY = Math.min(0, Math.max(ch - hS, imgY));
        srcImg.style.transform = `translate(${imgX}px,${imgY}px) scale(${scale})`;
        scaleSlider.value = Math.round(scale*100);
      }

      function reverseByte(b) {
        b = ((b & 0xF0)>>4)|((b & 0x0F)<<4);
        b = ((b & 0xCC)>>2)|((b & 0x33)<<2);
        b = ((b & 0xAA)>>1)|((b & 0x55)<<1);
        return b & 0xFF;
      }

      function getMaskCoord(xDest, yDest) {
        let mx = xDest, my = yDest;
        if (isLandscape) { mx = yDest; my = destW - 1 - xDest; }
        const rawX = -imgX/scale, rawY = -imgY/scale;
        let sx = Math.floor(rawX + mx/scale);
        let sy = Math.floor(rawY + my/scale);
        sx = Math.max(0, Math.min(srcImg.naturalWidth-1, sx));
        sy = Math.max(0, Math.min(srcImg.naturalHeight-1, sy));
        return { sx, sy };
      }

      importBtn.addEventListener('click', () => fileInput.click());
      fileInput.addEventListener('change', e => {
        const f = e.target.files[0];
        if (!f) return;
        if (!['image/png','image/jpeg'].includes(f.type)) {
          alert('Only PNG/JPEG allowed');
          return;
        }
        const reader = new FileReader();
        reader.onload = ev => {
          imgContainer.innerHTML = '';
          const img = document.createElement('img');
          img.src = ev.target.result;
          imgContainer.appendChild(img);
          srcImg = img;
          img.onload = () => {
            imgX = imgY = 0; scale = 1;
            clamp();
            recalcScaleLimits();
          };
        };
        reader.readAsDataURL(f);
      });

      btnToggleOri.addEventListener('click', () => {
        isLandscape = !isLandscape;
        imgContainer.classList.toggle('landscape', isLandscape);
        btnToggleOri.innerText = isLandscape ? 'Portrait' : 'Landscape';
        clamp();
        recalcScaleLimits();
      });

      scaleSlider.addEventListener('input', () => {
        if (!srcImg) return;
        scale = scaleSlider.value/100;
        clamp();
      });

      imgContainer.addEventListener('pointerdown', onPointerDown);
      imgContainer.addEventListener('pointermove', onPointerMove, {passive:false});
      imgContainer.addEventListener('pointerup',     onPointerUp);
      imgContainer.addEventListener('pointercancel', onPointerUp);
      imgContainer.addEventListener('pointerleave',  onPointerUp);

      imgContainer.addEventListener('touchstart', onTouchStart, {passive:false});
      imgContainer.addEventListener('touchmove',  onTouchMove,  {passive:false});
      imgContainer.addEventListener('touchend',   onTouchEnd);
      imgContainer.addEventListener('touchcancel',onTouchEnd);

      imgContainer.addEventListener('wheel', e => {
        e.preventDefault();
        if (!srcImg) return;
        const cw = imgContainer.clientWidth, ch = imgContainer.clientHeight;
        const minS = Math.max(cw/srcImg.naturalWidth, ch/srcImg.naturalHeight);
        const maxS = Math.min(10, srcImg.naturalWidth/cw, srcImg.naturalHeight/ch);
        const rect = imgContainer.getBoundingClientRect();
        const cx = e.clientX - rect.left, cy = e.clientY - rect.top;
        const factor = Math.exp(-e.deltaY*0.002);
        let newScale = scale * factor;
        newScale = Math.max(minS, Math.min(maxS, newScale));
        imgX = cx - (cx - imgX)*(newScale/scale);
        imgY = cy - (cy - imgY)*(newScale/scale);
        scale = newScale;
        clamp();
      }, {passive:false});

      function onPointerDown(e) {
        if (!srcImg) return;
        imgContainer.setPointerCapture(e.pointerId);
        pointers.set(e.pointerId, {x:e.clientX,y:e.clientY});
        if (pointers.size===2) {
          const [p1,p2]=Array.from(pointers.values());
          initialPinchDistance=Math.hypot(p1.x-p2.x,p1.y-p2.y);
          pinchStartScale=scale;
          const rect=imgContainer.getBoundingClientRect();
          pinchCenter={x:(p1.x+p2.x)/2-rect.left,y:(p1.y+p2.y)/2-rect.top};
        } else {
          dragging=true;
          const pt=pointers.get(e.pointerId);
          startX=pt.x-imgX; startY=pt.y-imgY;
          srcImg.classList.add('dragging');
        }
      }
      function onPointerMove(e) {
        if (!srcImg||!pointers.has(e.pointerId)) return;
        pointers.set(e.pointerId,{x:e.clientX,y:e.clientY});
        if (pointers.size===2) {
          e.preventDefault();
          const [p1,p2]=Array.from(pointers.values());
          const newDist=Math.hypot(p1.x-p2.x,p1.y-p2.y);
          let newScale=pinchStartScale*(newDist/initialPinchDistance);
          const cw=imgContainer.clientWidth,ch=imgContainer.clientHeight;
          const minS=Math.max(cw/srcImg.naturalWidth,ch/srcImg.naturalHeight);
          const maxS=Math.min(10,srcImg.naturalWidth/cw,srcImg.naturalHeight/ch);
          newScale=Math.max(minS,Math.min(maxS,newScale));
          imgX=pinchCenter.x-(pinchCenter.x-imgX)*(newScale/scale);
          imgY=pinchCenter.y-(pinchCenter.y-imgY)*(newScale/scale);
          scale=newScale; clamp();
        } else if (dragging) {
          e.preventDefault();
          const pt=pointers.get(e.pointerId);
          imgX=pt.x-startX; imgY=pt.y-startY; clamp();
        }
      }
      function onPointerUp(e) {
        pointers.delete(e.pointerId);
        imgContainer.releasePointerCapture(e.pointerId);
        if (dragging) { dragging=false; srcImg.classList.remove('dragging'); }
      }
      function onTouchStart(e) {
        if (!srcImg) return;
        e.preventDefault();
        const t=e.touches;
        if (t.length===2) {
          initialPinchDistance=Math.hypot(t[0].clientX-t[1].clientX,t[0].clientY-t[1].clientY);
          pinchStartScale=scale;
          const rect=imgContainer.getBoundingClientRect();
          pinchCenter={x:(t[0].clientX+t[1].clientX)/2-rect.left,
                      y:(t[0].clientY+t[1].clientY)/2-rect.top};
        } else if (t.length===1) {
          dragging=true;
          startX=t[0].clientX-imgX; startY=t[0].clientY-imgY;
          srcImg.classList.add('dragging');
        }
      }
      function onTouchMove(e) {
        if (!srcImg) return;
        e.preventDefault();
        const t=e.touches;
        if (t.length===2) {
          const newDist=Math.hypot(t[0].clientX-t[1].clientX,t[0].clientY-t[1].clientY);
          let newScale=pinchStartScale*(newDist/initialPinchDistance);
          const cw=imgContainer.clientWidth,ch=imgContainer.clientHeight;
          const minS=Math.max(cw/srcImg.naturalWidth,ch/srcImg.naturalHeight);
          const maxS=Math.min(10,srcImg.naturalWidth/cw,srcImg.naturalHeight/ch);
          newScale=Math.max(minS,Math.min(maxS,newScale));
          imgX=pinchCenter.x-(pinchCenter.x-imgX)*(newScale/scale);
          imgY=pinchCenter.y-(pinchCenter.y-imgY)*(newScale/scale);
          scale=newScale; clamp();
        } else if (dragging && t.length===1) {
          imgX=t[0].clientX-startX; imgY=t[0].clientY-startY; clamp();
        }
      }
      function onTouchEnd(e) {
        if (dragging) { dragging=false; srcImg.classList.remove('dragging'); }
      }

      function procImg(threshold, redMode) {
        if (!srcImg) { alert('Import first'); return; }
        const thr    = +document.getElementById('thresholdSlider').value;
        const redThr = +document.getElementById('redThresholdSlider').value;
        const bright = +document.getElementById('brightnessSlider').value;
        const contr  = +document.getElementById('contrastSlider').value / 100;
        const inv    = document.getElementById('invertCheckbox').checked;
        const pal    = redMode ? palColor : palBW;
        const W = destW, H = destH;

        const adj = new Uint8ClampedArray(256);
        for (let i = 0; i < 256; i++) {
          let v = (i - 128) * contr + 128 + bright;
          v = Math.max(0, Math.min(255, v));
          if (inv) v = 255 - v;
          adj[i] = v;
        }

        const off = document.createElement('canvas');
        off.width  = srcImg.naturalWidth;
        off.height = srcImg.naturalHeight;
        const offCtx = off.getContext('2d');
        offCtx.drawImage(srcImg, 0, 0);
        const pSrc = offCtx.getImageData(0, 0, off.width, off.height).data;

        const imgData = new ImageData(W, H);
        const data = imgData.data;

        if (threshold) {
          let idx = 0;
          for (let y = 0; y < H; y++) {
            for (let x = 0; x < W; x++) {
              const { sx, sy } = getMaskCoord(x, y);
              const pos = (sy * off.width + sx) * 4;
              const rVal = adj[pSrc[pos]],
                    gVal = adj[pSrc[pos+1]],
                    bVal = adj[pSrc[pos+2]];
              let ci;
              if (redMode && rVal >= redThr) {
                ci = 2;
              } else {
                const gray = (rVal + gVal + bVal) / 3;
                ci = gray < thr ? 0 : 1;
              }
              data[idx++] = pal[ci][0];
              data[idx++] = pal[ci][1];
              data[idx++] = pal[ci][2];
              data[idx++] = 255;
            }
          }
        } else {
          let idx = 0;
          const errArr = Array(H).fill().map(() => Array(W).fill([0,0,0]));
          for (let y = 0; y < H; y++) {
            for (let x = 0; x < W; x++) {
              const { sx, sy } = getMaskCoord(x, y);
              const pos = (sy * off.width + sx) * 4;
              const oldR = adj[pSrc[pos]]     + errArr[y][x][0],
                    oldG = adj[pSrc[pos+1]] + errArr[y][x][1],
                    oldB = adj[pSrc[pos+2]] + errArr[y][x][2];
              let best = 0, be = Infinity;
              for (let k = 0; k < pal.length; k++) {
                const dr = oldR - pal[k][0],
                      dg = oldG - pal[k][1],
                      db = oldB - pal[k][2],
                      e  = dr*dr + dg*dg + db*db;
                if (e < be) { be = e; best = k; }
              }
              const col = pal[best];
              data[idx++] = col[0];
              data[idx++] = col[1];
              data[idx++] = col[2];
              data[idx++] = 255;
              const er = (oldR - col[0]) / 8,
                    eg = (oldG - col[1]) / 8,
                    eb = (oldB - col[2]) / 8;
              const pushError = (yy, xx) => {
                if (yy < 0 || yy >= H || xx < 0 || xx >= W) return;
                const e0 = errArr[yy][xx];
                errArr[yy][xx] = [e0[0] + er, e0[1] + eg, e0[2] + eb];
              };
              pushError(y,   x+1); pushError(y,   x+2);
              pushError(y+1, x-1); pushError(y+1, x); pushError(y+1, x+1);
              pushError(y+2, x);
            }
          }
        }

        dstBox.innerHTML = '<canvas id="canvas"></canvas>';
        const c = document.getElementById('canvas'),
              ctx = c.getContext('2d');
        c.width  = W; c.height = H;
        ctx.putImageData(imgData, 0, 0);
      }

      function uploadImage() {
        const srcCanvas = document.getElementById('canvas');
        if (!srcCanvas) { alert('Generate first!'); return; }

        const W = srcCanvas.width, H = srcCanvas.height;
        const tmp = document.createElement('canvas');
        tmp.width  = W;
        tmp.height = H;
        const tctx = tmp.getContext('2d');

        tctx.translate(W/2, H/2);
        tctx.rotate(Math.PI);     
        tctx.drawImage(srcCanvas, -W/2, -H/2);

        const data  = tctx.getImageData(0, 0, W, H).data;
        const total = W*H;
        const bwPlane = new Uint8Array((total + 7) >> 3);
        const rdPlane = new Uint8Array((total + 7) >> 3);

        for (let pi = 0, i = 0; i < data.length; i += 4, pi++) {
          const r = data[i], g = data[i+1], b = data[i+2];
          const bit = 1 << (pi & 7), idx = pi >> 3;
          if (r > g && r > b) {
            rdPlane[idx] |= bit;
          } else {
            const gray = (r + g + b)/3;
            if (gray < 128) bwPlane[idx] |= bit;
          }
        }

        for (let i = 0; i < bwPlane.length; i++) {
          bwPlane[i] = reverseByte((~bwPlane[i]) & 0xFF);
          rdPlane[i] = reverseByte((~rdPlane[i]) & 0xFF);
        }

        fetch('/upload_bw', {
          method: 'POST',
          headers: { 'Content-Type':'application/octet-stream' },
          body: bwPlane
        })
        .then(r => { if (!r.ok) throw 'BW upload failed'; })
        .then(() => fetch('/upload_red', {
          method: 'POST',
          headers: { 'Content-Type':'application/octet-stream' },
          body: rdPlane
        }))
        .then(r => { if (!r.ok) throw 'RED upload failed'; })
        .then(() => fetch('/display', { method:'POST' }))
        .then(r => { if (!r.ok) throw 'Display failed'; alert('Sent!'); })
        .catch(err => alert('Error: ' + err));
      }


      document.getElementById('btnThreshMono').addEventListener('click',  () => procImg(true,  false));
      document.getElementById('btnThreshColor').addEventListener('click',() => procImg(true,  true));
      document.getElementById('btnDitherMono').addEventListener('click',  () => procImg(false, false));
      document.getElementById('btnDitherColor').addEventListener('click',() => procImg(false, true));
      document.getElementById('btnUpload').addEventListener('click',    uploadImage);
    </script>
  </body>
  </html>
)rawliteral";


