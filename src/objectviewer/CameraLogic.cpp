#include "objectviewer/CameraLogic.hpp"

namespace CameraLogic{
    Vertex sub(const Vertex& a, const Vertex& b){return {a.x - b.x, a.y - b.y, a.z - b.z};}
    float dot(const Vertex& a, const Vertex& b){return a.x * b.x + a.y * b.y + a.z * b.z;}

    Vertex cross(const Vertex& a, const Vertex& b){
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float length(const Vertex& v){return sqrt(dot(v, v));}
    
    Vertex normalize(const Vertex& v)
    {
        float len = length(v);
        if (len == 0.0f)
        {
            return {0.0f, 0.0f, 0.0f};
        }

        return {v.x / len, v.y / len, v.z / len};
    }

    Vertex getMeshCenter(const Mesh& mesh){
        BoundingBox box = mesh.findBoundingBox();

        return {
            (box.min.x + box.max.x) * 0.5f,
            (box.min.y + box.max.y) * 0.5f,
            (box.min.z + box.max.z) * 0.5f
        };
    }

    float getMeshRadius(const Mesh& mesh){
        BoundingBox box = mesh.findBoundingBox();

        float dx = box.max.x - box.min.x;
        float dy = box.max.y - box.min.y;
        float dz = box.max.z - box.min.z;

        return sqrt(dx * dx + dy * dy + dz * dz) * 0.5f;
    }

    Vertex getCameraPosition(const Camera& cam){
        Vertex pos;
        pos.x = cam.target.x + cam.distance * cos(cam.pitch) * sin(cam.yaw);
        pos.y = cam.target.y + cam.distance * sin(cam.pitch);
        pos.z = cam.target.z + cam.distance * cos(cam.pitch) * cos(cam.yaw);
        return pos;
    }


    // Fungsi untuk mengubah koordinat dunia ke koordinat kamera
    Vertex worldToCamera(const Vertex& point, const Camera& cam){
        Vertex camPos = getCameraPosition(cam);

        Vertex forward = normalize(sub(cam.target, camPos));
        Vertex worldUp = {0.0f, 1.0f, 0.0f};
        Vertex right = normalize(cross(forward, worldUp));
        Vertex up = cross(right, forward);

        Vertex rel = sub(point, camPos);

        return {
            dot(rel, right),
            dot(rel, up),
            dot(rel, forward)
        };
    }
    
    // Fungsi untuk memproyeksikan titik 3D ke layar 2D
    ScreenPoint projectPoint(const Vertex& point, const Camera& cam, const Viewport& viewport){
        ScreenPoint screen;

        Vertex camPoint = worldToCamera(point, cam);

        if (camPoint.z <= cam.nearPlane || camPoint.z >= cam.farPlane){
            screen.visible = false;
            return screen;
        }
        
        // Ubah FOV jadi radian
        float fovRad = cam.fov * 3.1415926535f / 180.0f;
        float focal = 1.0f / tan(fovRad * 0.5f);
        
        // Perspektif
        float xScreen = (camPoint.x * focal) / camPoint.z;
        float yScreen = (camPoint.y * focal) / camPoint.z;
        
        // Ubah jadi screen pixel per point
        screen.x = viewport.width * 0.5f + xScreen * viewport.width * 0.5f;
        screen.y = viewport.height * 0.5f - yScreen * viewport.height * 0.5f;
        screen.z = camPoint.z;
        screen.visible = true;

        return screen;
    }
}