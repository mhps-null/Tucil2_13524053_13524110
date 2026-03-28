#include "objectviewer/Viewer.hpp"

struct RenderFace
{
    ScreenPoint p1, p2, p3;
    float depth;
    Color color;
};

void Viewer::show(const Mesh& mesh){
    Viewport viewport{1000, 700};
    RenderWindow window(VideoMode(viewport.width, viewport.height), "Object Viewer");
    window.setFramerateLimit(60);

    Camera cam;
    cam.target = CameraLogic::getMeshCenter(mesh);

    float radius = CameraLogic::getMeshRadius(mesh);
    if (radius < 1.0f) radius = 1.0f;

    cam.distance = radius * 3.0f;
    cam.yaw = 0.8f;
    cam.pitch = 0.4f;
    cam.fov = 90.0f;
    cam.nearPlane = 0.1f;
    cam.farPlane = 1000.0f;

    bool dragging = false;
    Vector2i lastMouse;
    
    vector<geometry::Vertex> cameraVertices(mesh.vertices.size());
    vector<ScreenPoint> projected(mesh.vertices.size());
    vector<RenderFace> renderFaces;
    renderFaces.reserve(mesh.faces.size());

    while (window.isOpen()){
        Event event;
        while (window.pollEvent(event)){
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left){
                dragging = true;
                lastMouse = Mouse::getPosition(window);
            }

            if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left){
                dragging = false;
            }

            if (event.type == Event::MouseWheelScrolled){
                cam.distance -= event.mouseWheelScroll.delta * radius * 0.2f;

                if (cam.distance < radius * 0.5f) cam.distance = radius * 0.5f;
                if (cam.distance > radius * 10.0f) cam.distance = radius * 10.0f;
            }
        }

        if (dragging){
            Vector2i currentMouse = Mouse::getPosition(window);
            Vector2i delta = currentMouse - lastMouse;
            lastMouse = currentMouse;

            cam.yaw += delta.x * 0.01f;
            cam.pitch -= delta.y * 0.01f;

            if (cam.pitch > 1.5f) cam.pitch = 1.5f;
            if (cam.pitch < -1.5f) cam.pitch = -1.5f;
        }
        
        /**
         * Hotkeys dengan Keyboard:
         * 1. WASD buat move geser-geser biasa.
         * 2. IO untuk zoom in dan zoom out.
         */

        float moveSpeed = radius * 0.02f;
        float zoomSpeed = radius * 0.05f;

        if (Keyboard::isKeyPressed(Keyboard::A)) cam.target.x -= moveSpeed;
        if (Keyboard::isKeyPressed(Keyboard::D)) cam.target.x += moveSpeed;
        if (Keyboard::isKeyPressed(Keyboard::S)) cam.target.y -= moveSpeed;
        if (Keyboard::isKeyPressed(Keyboard::W)) cam.target.y += moveSpeed;
        
        if (Keyboard::isKeyPressed(Keyboard::I)) cam.distance -= zoomSpeed;
        if (Keyboard::isKeyPressed(Keyboard::O)) cam.distance += zoomSpeed;

        if (cam.distance < radius * 0.5f) cam.distance = radius * 0.5f;
        if (cam.distance > radius * 10.0f) cam.distance = radius * 10.0f;

        window.clear(Color::Black);


        for (size_t i = 0; i < mesh.vertices.size(); i++)
        {
            geometry::Vertex world = (mesh.vertices[i]);
            cameraVertices[i] = CameraLogic::worldToCamera(world, cam);
            projected[i] = CameraLogic::projectPoint(world, cam, viewport);
        }

        renderFaces.reserve(mesh.faces.size());

        for (const Face& face : mesh.faces)
        {
            const geometry::Vertex& c1 = cameraVertices[face.v1];
            const geometry::Vertex& c2 = cameraVertices[face.v2];
            const geometry::Vertex& c3 = cameraVertices[face.v3];

            const ScreenPoint& p1 = projected[face.v1];
            const ScreenPoint& p2 = projected[face.v2];
            const ScreenPoint& p3 = projected[face.v3];

            if (!p1.visible || !p2.visible || !p3.visible) continue;

            geometry::Vertex ab = CameraLogic::sub(c2, c1);
            geometry::Vertex ac = CameraLogic::sub(c3, c1);
            geometry::Vertex normal = CameraLogic::cross(ab, ac);

            if (CameraLogic::dot(normal, c1) >= 0) continue;

            float depth = (c1.z + c2.z + c3.z) / 3.0f;

            geometry::Vertex lightDir = CameraLogic::normalize(geometry::Vertex{0.5f, 1.0f, -0.8f});
            geometry::Vertex n = CameraLogic::normalize(normal);
            float brightness = max(0.2f, (CameraLogic::dot(n, lightDir)));

            Uint8 c = static_cast<Uint8>(brightness * 220.0f);

            renderFaces.push_back({p1, p2, p3, depth, Color(c, c, c)});

            // renderFaces.push_back({p1, p2, p3, depth, Color(180, 180, 180)});
        }

        sort(renderFaces.begin(), renderFaces.end(), [](const RenderFace& a, const RenderFace& b){
            return a.depth > b.depth;
        });

        VertexArray triangles(Triangles);
        triangles.resize(renderFaces.size() * 3);

        // Langsung memetakan renderFaces ke VertexArray global biar langsung digambar sekaligus
        for (size_t i = 0; i < renderFaces.size(); i++){
            const auto& rf = renderFaces[i];

            triangles[i * 3 + 0].position = Vector2f(rf.p1.x, rf.p1.y);
            triangles[i * 3 + 0].color = rf.color;

            triangles[i * 3 + 1].position = Vector2f(rf.p2.x, rf.p2.y);
            triangles[i * 3 + 1].color = rf.color;

            triangles[i * 3 + 2].position = Vector2f(rf.p3.x, rf.p3.y);
            triangles[i * 3 + 2].color = rf.color;
        }
        
        window.draw(triangles);

        window.display();
        renderFaces.clear();
    }
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        cerr << "Not the format, do viewer <path_to_obj>.\n";
        return 1;
    }

    string filePath = argv[1];

    Mesh mesh;
    if (!FileSystem::loadObjFromFile(filePath, mesh)){
        cerr << "Failed to load from " << filePath << ".\n";
        return 1;
    }

    if (mesh.vertices.empty() || mesh.faces.empty()){
        cerr << "Mesh is empty.\n";
        return 1;
    }

    Viewer::show(mesh);
    return 0;
}