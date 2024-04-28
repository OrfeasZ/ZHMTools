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
const checkEdges = document.getElementById('check-edges');
const checkBasisVertices = document.getElementById('check-basis-vertices');
const checkPortalVertices = document.getElementById('check-portal-vertices');
const checkBBox = document.getElementById('check-bbox');
const checkNumbers = document.getElementById('check-numbers');
const checkKDTree = document.getElementById('check-kdtree');
const useOrderedEdgeColors = document.getElementById('check-use-ordered-edge-colors');
const highlightArea = document.getElementById('highlight-area');
const kdTreeDepth = document.getElementById('kd-tree-depth');

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

function renderPortalVert(edges) {
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

function setLineSegmentColor(lineSegmentIndex, colors, hasAdjacentArea) {
    let lineSegmentColor; 
    if (hasAdjacentArea != 0) {
        lineSegmentColor = [0.0, 0.0, 1.0];
    } else {
        lineSegmentColor = [0.0, 1.0, 0.0];
    }
    if (lineSegmentIndex == 0) {
        colors[lineSegmentIndex * 6] = 1.0;
        colors[lineSegmentIndex * 6 + 1] = 1.0;
        colors[lineSegmentIndex * 6 + 2] = 1.0;
    } else {
        colors[lineSegmentIndex * 6] = lineSegmentColor[0];
        colors[lineSegmentIndex * 6 + 1] = lineSegmentColor[1];
        colors[lineSegmentIndex * 6 + 2] = lineSegmentColor[2];
    }
    colors[lineSegmentIndex * 6 + 3] = lineSegmentColor[0];
    colors[lineSegmentIndex * 6 + 4] = lineSegmentColor[1];
    colors[lineSegmentIndex * 6 + 5] = lineSegmentColor[2];
}

function renderText(number, x, y, z) {
    const text = new THREE.TextSprite({
        alignment: 'center',
        color: '#ffffff',
        fontFamily: 'Arial',
        fontSize: .5,
        text: number.toString(),
    });

    text.position.set(y, z + 0.16, x);

    scene.add(text);
}

function renderEdges(edges, showEdgeNumbers) {
    if (showEdgeNumbers) {
        let edgeIndex = 0;
        for (const edge of edges) {
            renderText(edgeIndex, edge[0], edge[1], edge[2]);
            edgeIndex++;
        }
    }
        
    if (!useOrderedEdgeColors.checked) {
        const points = [];

        for (const edge of edges) {
            points.push(new THREE.Vector3(edge[1], edge[2], edge[0]));
        }

        points.push(new THREE.Vector3(edges[0][1], edges[0][2], edges[0][0]));

        const geometry = new THREE.BufferGeometry().setFromPoints(points);

        const lineMaterial = new THREE.LineBasicMaterial({ color: 0x00ff00 });
        const line = new THREE.Line(geometry, lineMaterial);

        scene.add(line);
    } else {
        const points = [];
        const colors = new Float32Array((edges.length + 1) * 6);
        let edgeIndex = 0;
        let lineSegmentIndex = 0;

        for (const edge of edges) {
            points.push(new THREE.Vector3(edge[1], edge[2], edge[0]));
            setLineSegmentColor(lineSegmentIndex, colors, edge[4]);
            lineSegmentIndex++;
            if (edgeIndex != 0) {
                points.push(new THREE.Vector3(edge[1], edge[2], edge[0]));
            }
            edgeIndex++;
        }

        points.push(new THREE.Vector3(edges[0][1], edges[0][2], edges[0][0]));
        setLineSegmentColor(lineSegmentIndex, colors, 0);

        const geometry = new THREE.BufferGeometry().setFromPoints(points);
        geometry.setAttribute("color", new THREE.BufferAttribute(colors, 3));

        const lineMaterial = new THREE.LineBasicMaterial({ vertexColors: THREE.VertexColors });
        lines = new THREE.Object3D();
	    line = new THREE.LineSegments( geometry,  lineMaterial);
        lines.add(line);
	    scene.add(lines);
    }
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

function renderBasisVert(faceIdx, vertex) {
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

    if (parseInt(highlightArea.value) == i) {
        faceColor = 0xFF0000;
    }

    if (checkFaces.checked) {
        renderFace(vertices, faceColor);
    }
    
    if (checkEdges.checked) {
        renderEdges(vertices, parseInt(highlightArea.value) == i);
    }

    if (checkBasisVertices.checked) {
        renderBasisVert(i, vertices[vertexIndex]);
    }

    if (checkPortalVertices.checked) {
        renderPortalVert(vertices);
    }

    if (checkCenters.checked) {
        renderSurfaceCenter(centerX, centerY, centerZ);
    }

    if (checkNumbers.checked) {
        renderText(i, centerX, centerY, centerZ);
    }
}

function renderBBox(unk) {
    const box = new THREE.Box3(new THREE.Vector3(unk[1], unk[2] + 0.01, unk[0]), new THREE.Vector3(unk[4], unk[5] + 0.01, unk[3]));

    const helper = new THREE.Box3Helper(box, 0xff00ff);
    scene.add(helper);
}

function getSplitAxisColor(splitAxis) {
    let color;
    if (splitAxis == 0) {
        color = 0x000080;
    } else if (splitAxis == 1) {
        color = 0x008000;
    } else {
        color = 0x800000;
    }
    return color;
}

function renderBBoxWithColor(color, bbox) {
    const box = new THREE.Box3(new THREE.Vector3(bbox[1], bbox[2] + 0.01, bbox[0]), new THREE.Vector3(bbox[4], bbox[5] + 0.01, bbox[3]));
    const helper = new THREE.Box3Helper(box, color);
    scene.add(helper);
}

function renderAxes() {
        const axes = [["X", [1,0,0]], ["Y", [0,1,0]], ["Z", [0,0,1]]]
        let color = 0x0000ff;
        for (const axis of axes) {
            const points = [];
            points.push(new THREE.Vector3(0, 0, 0), new THREE.Vector3(axis[1][1], axis[1][2], axis[1][0]));
            
            const geometry = new THREE.BufferGeometry().setFromPoints(points);

            const lineMaterial = new THREE.LineBasicMaterial({ color:color });
            const line = new THREE.Line(geometry, lineMaterial);

            scene.add(line);
            color *= 256;
            renderText(axis[0], axis[1][0], axis[1][1], axis[1][2]);
        }

}
controls.update();

function animate() {
    requestAnimationFrame(animate);

    controls.update();

    renderer.render(scene, camera);
}

animate();

function resetCamera() {
    const selectedMap = mapSelector.value;
    camera.position.set(-40, 50, 80);
}

function reRender() {
    // Clear the scene.
    while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
    }

    const selectedMap = mapSelector.value;
    console.log('Rendering ' + selectedMap);

    if (checkBBox.checked) {
        renderBBox(Areas[selectedMap][0]);
    }

    for (let i = 1; i < Areas[selectedMap].length; ++i) {
        const surface = Areas[selectedMap][i];
        renderSurface(i, surface);
    }
    
    let maxDepth = -1;
    for (const [depth, bboxes] of Object.entries(KDTree[selectedMap])) {
        maxDepth = Math.max(maxDepth, depth);
    }
    kdTreeDepth.min = 0;
    kdTreeDepth.max = maxDepth;

    if (checkKDTree.checked) {
        
        for (const [depth, bboxes] of Object.entries(KDTree[selectedMap]))
        {
            if (kdTreeDepth.value == -1 || kdTreeDepth.value == depth) {
                let bboxIndex = 0;
                for (const bbox of bboxes) {
                    const color = getSplitAxisColor(bbox[6]);
                    renderBBoxWithColor(color, bbox);
                    bboxIndex++;
                }
            }
        }
    }
    renderAxes();
}

let hasSelection = false;

for (const mapName in Areas) {
    const optionElement = document.createElement('option');
    optionElement.innerHTML = mapName;

    if (!hasSelection) {
        optionElement.selected = true;
        hasSelection = true;
    }

    mapSelector.appendChild(optionElement);
}

resetCamera();
reRender();

mapSelector.addEventListener('change', () => { resetCamera(); reRender()});
checkRadii.addEventListener('change', () => reRender());
useOrderedEdgeColors.addEventListener('change', () => reRender());
highlightArea.addEventListener('input', () => reRender());
kdTreeDepth.addEventListener('input', () => reRender());
checkCenters.addEventListener('change', () => reRender());
checkFaces.addEventListener('change', () => reRender());
checkEdges.addEventListener('change', () => reRender());
checkBasisVertices.addEventListener('change', () => reRender());
checkPortalVertices.addEventListener('change', () => reRender());
checkBBox.addEventListener('change', () => reRender());
checkNumbers.addEventListener('change', () => reRender());
checkKDTree.addEventListener('change', () => reRender());
