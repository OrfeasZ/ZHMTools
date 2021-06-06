const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);

const rendererElement = document.getElementById('render-canvas');
rendererElement.replaceWith(renderer.domElement);

const mapSelector = document.getElementById('map-selector');

const checkRadii = document.getElementById('check-radii');
const checkCenters = document.getElementById('check-centers');
const checkFaces = document.getElementById('check-faces');
const checkLines = document.getElementById('check-lines');
const checkMarkedVertices = document.getElementById('check-marked-vertices');
const checkWeirdVertices = document.getElementById('check-weird-vertices');
const checkAABB = document.getElementById('check-aabb');
const checkNumbers = document.getElementById('check-numbers');

const controls = new THREE.OrbitControls(camera, renderer.domElement);

function renderTriangle(v1, v2, v3, color) {
    const geometry = new THREE.BufferGeometry();

    const vertices = [
        v1[1], v1[2], v1[0],
        v2[1], v2[2], v2[0],
        v3[1], v3[2], v3[0],
    ];

    geometry.setAttribute('position', new THREE.BufferAttribute(new Float32Array(vertices), 3));

    const meshMaterial = new THREE.MeshBasicMaterial({ color, side: THREE.DoubleSide, opacity: 0.5, transparent: true, blending: THREE.AdditiveBlending });
    const mesh = new THREE.Mesh(geometry, meshMaterial);

    scene.add(mesh);
}

function renderFace(edges, color) {
    for (let i = 0; i < edges.length - 2; ++i) {
        renderTriangle(edges[0], edges[i + 1], edges[i + 2], color);
    }
}

function renderWeirdEdges(edges) {
    for (const edge of edges) {
        if (edge[3] !== 1) {
            continue;
        }
        
        const edgeColor = 0x0000ff;
        
        const geometry = new THREE.CircleGeometry(0.1, 5);
        const material = new THREE.MeshBasicMaterial({ color: edgeColor, side: THREE.DoubleSide });

        const circle = new THREE.Mesh(geometry, material);
        circle.position.set(edge[1], edge[2] + 0.01, edge[0]);
        circle.rotation.set(1.5708, 0, 0);

        scene.add(circle);
    }
}

function renderOutline(edges) {
    const points = [];

    for (const edge of edges) {
        points.push(new THREE.Vector3(edge[1], edge[2], edge[0]));
    }

    points.push(new THREE.Vector3(edges[0][1], edges[0][2], edges[0][0]));

    const geometry = new THREE.BufferGeometry().setFromPoints(points);

    const lineMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
    const line = new THREE.Line(geometry, lineMaterial);

    scene.add(line);
}

function renderSurfaceRadius(centerX, centerY, centerZ, maxRadius) {
    const geometry = new THREE.CircleGeometry(maxRadius, 32);
    const material = new THREE.MeshBasicMaterial({ color: 0x00ff00, side: THREE.DoubleSide, opacity: 0.3, transparent: true });

    const circle = new THREE.Mesh(geometry, material);
    circle.position.set(centerY, centerZ - 0.01, centerX);
    circle.rotation.set(1.5708, 0, 0);

    scene.add(circle);
}

function renderSurfaceCenter(centerX, centerY, centerZ) {
    const geometry = new THREE.CircleGeometry(0.1, 5);
    const material = new THREE.MeshBasicMaterial({ color: 0xff0000, side: THREE.DoubleSide });

    const circle = new THREE.Mesh(geometry, material);
    circle.position.set(centerY, centerZ + 0.01, centerX);
    circle.rotation.set(1.5708, 0, 0);

    scene.add(circle);
}

function renderMarkedVertex(faceIdx, vertex) {
    const geometry = new THREE.CircleGeometry(0.1, 5);
    const material = new THREE.MeshBasicMaterial({ color: 0xff00ff, side: THREE.DoubleSide });

    const circle = new THREE.Mesh(geometry, material);
    circle.position.set(vertex[1], vertex[2] + 0.01, vertex[0]);
    circle.rotation.set(1.5708, 0, 0);

    scene.add(circle);

    const text = new THREE.TextSprite({
        alignment: 'center',
        color: '#000000',
        fontFamily: 'Arial',
        fontSize: 0.12,
        text: faceIdx.toString(),
    });

    text.position.set(vertex[1], vertex[2] + 0.05, vertex[0]);

    scene.add(text);
}

function renderSurfaceNumber(number, x, y, z) {
    const text = new THREE.TextSprite({
        alignment: 'center',
        color: '#ffffff',
        fontFamily: 'Arial',
        fontSize: 0.2,
        text: number.toString(),
    });

    text.position.set(y, z + 0.05, x);

    scene.add(text);
}

function renderSurface(i, surface) {
    const centerX = surface[0];
    const centerY = surface[1];
    const centerZ = surface[2];
    const maxRadius = surface[3];

    const isSteps = surface[4] === 8;
    const vertexIndex = surface[5];
    const vertices = surface[6];

    if (checkRadii.checked) {
        renderSurfaceRadius(centerX, centerY, centerZ, maxRadius);
    }

    let faceColor = 0x00FF00;

    if (isSteps) {
        faceColor = 0xFF69B4;
    }

    if (checkFaces.checked) {
        renderFace(vertices, faceColor);
    }
    
    if (checkLines.checked) {
        renderOutline(vertices);
    }

    if (checkMarkedVertices.checked) {
        renderMarkedVertex(i, vertices[vertexIndex]);
    }

    if (checkWeirdVertices.checked) {
        renderWeirdEdges(vertices);
    }

    if (checkCenters.checked) {
        renderSurfaceCenter(centerX, centerY, centerZ);
    }

    if (checkNumbers.checked) {
        renderSurfaceNumber(i, centerX, centerY, centerZ);
    }
}

function renderAABB(unk) {
    const box = new THREE.Box3(new THREE.Vector3(unk[1], unk[2] + 0.01, unk[0]), new THREE.Vector3(unk[4], unk[5] + 0.01, unk[3]));

    const helper = new THREE.Box3Helper(box, 0xff00ff);
    scene.add(helper);
}

camera.position.set(-40, 50, 80);
controls.update();

function animate() {
    requestAnimationFrame(animate);

    controls.update();

    renderer.render(scene, camera);
}

animate();

function reRender() {
    // Clear the scene.
    while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
    }

    // Render everything.
    const selectedMap = mapSelector.value;

    console.log('Rendering ' + selectedMap);

    if (checkAABB.checked) {
        renderAABB(Surfaces[selectedMap][0]);
    }

    for (let i = 1; i < Surfaces[selectedMap].length; ++i) {
        const surface = Surfaces[selectedMap][i];
        renderSurface(i, surface);
    }
}

let hasSelection = false;

for (const mapName in Surfaces) {
    const optionElement = document.createElement('option');
    optionElement.innerHTML = mapName;

    if (!hasSelection) {
        optionElement.selected = true;
        hasSelection = true;
    }

    mapSelector.appendChild(optionElement);
}

reRender();

mapSelector.addEventListener('change', () => reRender());
checkRadii.addEventListener('change', () => reRender());
checkCenters.addEventListener('change', () => reRender());
checkFaces.addEventListener('change', () => reRender());
checkLines.addEventListener('change', () => reRender());
checkMarkedVertices.addEventListener('change', () => reRender());
checkWeirdVertices.addEventListener('change', () => reRender());
checkAABB.addEventListener('change', () => reRender());
checkNumbers.addEventListener('change', () => reRender());
