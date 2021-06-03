const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);

const controls = new THREE.OrbitControls(camera, renderer.domElement);

const size = 1000;
const divisions = 1000;

const gridHelper = new THREE.GridHelper(size, divisions);
//scene.add(gridHelper);

function renderTriangle(v1, v2, v3) {
    const geometry = new THREE.BufferGeometry();

    const vertices = [
        v1[1], v1[2], v1[0],
        v2[1], v2[2], v2[0],
        v3[1], v3[2], v3[0],
    ];

    geometry.setAttribute('position', new THREE.BufferAttribute(new Float32Array(vertices), 3));

    const meshMaterial = new THREE.MeshBasicMaterial({ color: 0xFF69B4, side: THREE.DoubleSide, opacity: 0.3, transparent: true, blending: THREE.AdditiveBlending });
    const mesh = new THREE.Mesh(geometry, meshMaterial);

    scene.add(mesh);
}

function renderFace(edges) {
    for (let i = 0; i < edges.length - 2; ++i) {
        renderTriangle(edges[0], edges[i + 1], edges[i + 2]);
    }
}

function renderOutline(edges) {
    const points = [];

    for (const edge of edges) {
        points.push(new THREE.Vector3(edge[1], edge[2], edge[0]));
    }

    points.push(new THREE.Vector3(edges[0][1], edges[0][2], edges[0][0]));

    const geometry = new THREE.BufferGeometry().setFromPoints(points);

    const lineMaterial = new THREE.LineBasicMaterial({ color: 0xFF69B4 });
    const line = new THREE.Line(geometry, lineMaterial);

    scene.add(line);
}

for (const face of Faces) {
    renderFace(face);
    renderOutline(face);
}

camera.position.set(-40, 50, 80);
controls.update();

function animate() {
    requestAnimationFrame(animate);

    controls.update();

    renderer.render(scene, camera);
}
animate();