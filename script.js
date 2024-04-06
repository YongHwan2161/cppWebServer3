let user = 0, node = 0, ch = 0;
function updateClock() {
    document.getElementById("clock").textContent = new Date().toLocaleTimeString();
}

function sendTextToServer(textinput) {
    //const textinput = document.getElementById("textinput").value;
    const fileContent = document.getElementById('fileContent').value;
    const additionalContent = textinput === "editHtml" ? `\t${fileContent}` : (textinput === "editcppl" ? `\t${fileContent}` : '');
    const text = `${user}\t${node}\t${ch}\t${textinput}${additionalContent}`;

    const xhr = new XMLHttpRequest();
    xhr.open('POST', 'https://obscure-space-train-v9xrjvg795p2rx5-8080.app.github.dev/', true);
    xhr.setRequestHeader('Content-Type', 'text/plain; charset=utf-8');
    xhr.onload = function () {
        if (xhr.status === 200) {
            const res = xhr.responseText.split('\t');
            updateUI(res);
        } else {
            console.error('Error:', xhr.status, xhr.statusText);
            document.getElementById("contents").textContent = `An error occurred: ${xhr.status} ${xhr.statusText}`;
        }
    };
    xhr.onerror = function () {
        console.error('Request failed.');
    };
    xhr.send(text);
    document.getElementById("textinput").focus();
}
function updateUI(res) {
    user = res[0];
    node = res[1];
    ch = res[2];
    const contents = document.getElementById("contents");
    const info = document.getElementById("info");
    if (res[5] === "html") {
        document.getElementById('fileContent').value = res[3];
    } else if (res[5] === "cpp") {
        document.getElementById('fileContent').value = res[3];
    } else {
        contents.innerHTML = res[3];
    }
    info.textContent = `user: ${res[0]}, node: ${res[1]}/${res[4]}, ch: ${res[2]}, Last: ${res[6]}`;
    document.getElementById("textinput").value = res[5];
    console.log(res);
}
function saveFileChanges() {
    if (document.getElementById("textinput").value === "html") {
        document.getElementById("textinput").value = "editHtml";
    } else {
        document.getElementById("textinput").value = "editcpp";
    }
    sendTextToServer();
    //location.reload();
}
window.onload = function () {
    document.getElementById('Log').innerText = 'Page is fully loaded.';
    updateClock();
    setInterval(updateClock, 1000);
    sendTextToServer();
}
document.getElementById("loginForm").addEventListener("submit", function (event) {
    event.preventDefault(); // Prevent the form from submitting

    var formData = new FormData(this); // Collect form data

    fetch("/login", {
        method: "POST",
        body: formData
    })
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.text();
        })
        .then(data => {
            // Handle successful login response
            const res = data.split('\t');
            updateUI(res);
            // Hide the login form
            document.getElementById("loginForm").style.display = "none";
            //console.log(data); // Do whatever processing you need with the response data
        })
        .catch(error => {
            // Handle error
            console.error('There was a problem with the fetch operation:', error);
        });
});

// WebGL 초기화
const canvas = document.getElementById('glCanvas');
const gl = canvas.getContext('webgl');

// 정육면체 버텍스 데이터
const vertices = [
// 앞면 (Front)
-1.0, -1.0,  1.0, // 왼쪽 아래
1.0, -1.0,  1.0, // 오른쪽 아래
1.0,  1.0,  1.0, // 오른쪽 위
-1.0,  1.0,  1.0, // 왼쪽 위

// 뒷면 (Back)
-1.0, -1.0, -1.0, // 왼쪽 아래
1.0, -1.0, -1.0, // 오른쪽 아래
1.0,  1.0, -1.0, // 오른쪽 위
-1.0,  1.0, -1.0, // 왼쪽 위
];

// 버텍스 인덱스
const indices = [
0, 1, 2,  // 앞면 삼각형 1
0, 2, 3,  // 앞면 삼각형 2
4, 5, 6,  // 뒷면 삼각형 1
4, 6, 7,  // 뒷면 삼각형 2
0, 3, 7,  // 왼쪽 삼각형 1
0, 7, 4,  // 왼쪽 삼각형 2
1, 2, 6,  // 오른쪽 삼각형 1
1, 6, 5,  // 오른쪽 삼각형 2
0, 1, 5,  // 아래 삼각형 1
0, 5, 4,  // 아래 삼각형 2
2, 3, 7,  // 위 삼각형 1
2, 7, 6   // 위 삼각형 2
];

// 버텍스 버퍼 생성
const vertexBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

// 인덱스 버퍼 생성
const indexBuffer = gl.createBuffer();
gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);

// 쉐이더 소스 코드
const vertexShaderSource = `
attribute vec3 aVertexPosition;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
void main(void) {
gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aVertexPosition, 1.0);
}
`;

const fragmentShaderSource = `
void main(void) {
gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
`;

// 쉐이더 컴파일
const vertexShader = gl.createShader(gl.VERTEX_SHADER);
gl.shaderSource(vertexShader, vertexShaderSource);
gl.compileShader(vertexShader);

const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
gl.shaderSource(fragmentShader, fragmentShaderSource);
gl.compileShader(fragmentShader);

// 프로그램 생성 및 연결
const shaderProgram = gl.createProgram();
gl.attachShader(shaderProgram, vertexShader);
gl.attachShader(shaderProgram, fragmentShader);
gl.linkProgram(shaderProgram);
gl.useProgram(shaderProgram);

// 버텍스 버퍼 설정
const vertexPositionAttribute = gl.getAttribLocation(shaderProgram, 'aVertexPosition');
gl.enableVertexAttribArray(vertexPositionAttribute);
gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
// glMatrix에서 mat4 객체 가져오기
const mat4 = glMatrix.mat4;
// 모델 뷰 매트릭스 및 투영 매트릭스 설정
const modelViewMatrix = mat4.create();
const projectionMatrix = mat4.create();
mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -5.0]); // 시야를 원점에서 (0, 0, -5)로 이동
mat4.rotate(modelViewMatrix, modelViewMatrix, Math.PI / 4, [0, 1, 0]); // y축을 기준으로 45도 회전
mat4.perspective(projectionMatrix, Math.PI / 4, canvas.clientWidth / canvas.clientHeight, 0.1, 100.0); // 투영 매트릭스 설정

// 유니폼 설정
const uModelViewMatrix = gl.getUniformLocation(shaderProgram, 'uModelViewMatrix');
const uProjectionMatrix = gl.getUniformLocation(shaderProgram, 'uProjectionMatrix');
gl.uniformMatrix4fv(uModelViewMatrix, false, modelViewMatrix);
gl.uniformMatrix4fv(uProjectionMatrix, false, projectionMatrix);

// 캔버스 클리어 및 렌더링
gl.clearColor(0.0, 0.0, 0.0, 1.0);
gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
gl.enable(gl.DEPTH_TEST);
gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);
