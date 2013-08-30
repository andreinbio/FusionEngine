#include "../utils/utils.hpp"

//#define SOLID_CUBE {0,8}
//#define EMPTY_CUBE {0,0}

typedef struct float_edge_span {
    glm::vec3 start, ending;
}float_edge_span;

typedef struct char_edge_span {
    unsigned char start, ending; //we will store here int numbers not chars...
}char_edge_span;

typedef struct face_corners {
    bool luc, lbc, ruc, rbc;//left upper corner, left bottom corner, right upper corner, right bottom corner
}face_corners;

typedef struct cube {
    cube *children;
    char_edge_span edges[12];
    bool solid;
    face_corners face[6];
}cube;

typedef struct data_selection {
    glm::vec3 vertices[8];
    GLubyte elements[16];
    glm::vec3 center;
    float radius;
}data_selection;

typedef struct cube_vertices {
    glm::vec3 vertices[8];
}cube_vertices;

typedef struct verts {
    glm::vec3 vertices[6];
}verts;

typedef struct vector_data {
    verts face[6];
}vector_data;

//typedef struct plane_points {
//    glm::vec3 a, b, c;
//    char orient;
//}plane_points;

//typedef struct face_axis {
//    plane_points X, Y, Z;
//}face_axis;

class OctreeManagement: public utils {

    private:
        cube *worldroot;
        glm::vec3 world_center;
        float world_radius;
        unsigned short level;

        char orientation;
        char selection_orientation;
        //float cube_radius;
        //glm::vec3 cube_center;

        //glm::vec3 PointerPosition;
        data_selection cube_selection;

        bool keyG;
        short mouse_scroll;

        bool on_click;
        bool selected;

        GLuint uniform_color;

        GLuint nr_vertices;

    public:
        //editing mode functions
        cube *new_cubes(bool consistency);
        glm::vec3 find_octree_center(glm::vec3 pointer_position, glm::vec3 cube_center, float radius);
        void updateCubeFace(glm::vec3 position, glm::vec3 direction, GLushort window_width, GLushort window_height, MVPmatrices matrix);
        bool subdivide_cube(cube &c);
        cube *return_selected_cube(unsigned short &child_cube, glm::vec3 cube_location);
        cube *return_neighbor_cube(unsigned short &i, glm::vec3 cube_location);
        void delete_children(cube &c);
        void set_edge_spans(cube &c);
        void set_all_faces_to_false(cube &c);
        void set_octree_face_visibility(cube *c);
        void set_cube_face_visibility(cube &c, unsigned short face_index, bool visibility);
        void recursively_set_cube_face_visibility(cube &c, unsigned short face_index, bool visibility);
        void set_children_face_visibility(cube *c, unsigned short face_index, unsigned short luc_cube, unsigned short lbc_cube, unsigned short ruc_cube, unsigned short rbc_cube);
        void GenerateOctreeData(short mouse_scroll);
        void update_cube_face_visibility(cube *c_parent, unsigned short child_cube, short mouse_scroll);
        unsigned short find_child_cube(glm::vec3 pointer_position, glm::vec3 cube_center);
        void translate_cube_center(short mouse_scroll);
        void generate_octree();
        cube *return_worldroot();
        //glm::vec3 return_intersection_point(glm::vec3 position, glm::vec3 direction);
        //glm::vec3 return_intersection_point2(glm::vec3 position, glm::vec3 direction, GLushort window_width, GLushort window_height, MVPmatrices matrix);
        //glm::vec3 line_cube_intersection(glm::vec3 center, float radius, glm::vec3 position, glm::vec3 direction);
        //glm::vec3 line_plane_intersection(glm::vec3 origin, glm::vec3 direction, glm::vec3 normal, glm::vec3 point_on_plane);
        //face_axis find_visible_faces(glm::vec3 center, glm::vec3 position, cube_vertices Vert);
        //void calculate_orientation(glm::vec3 point, glm::vec3 center, float radius);
        //bool is_octree_parent_solid(cube *octree_data, glm::vec3 center, float radius);

        //testing function
        void function_new_function(cube *c_parent);

        void verify_sibling_cube(cube *c_parent, unsigned short main_cube, unsigned short main_cube_face, unsigned short neighbor_cube, unsigned short neighbor_cube_face, short mouse_scroll);
        void verify_neighbor_cube(cube *main_c, unsigned short main_cube_face, unsigned short main_cube, short mouse_scroll);

        void verify_corner(bool &neighbor_corner, bool &main_corner,
                                        unsigned char neighbor_first_edge, unsigned char main_first_edge,
                                        unsigned char neighbor_second_edge, unsigned char main_second_edge,
                                        unsigned char neighbor_third_edge, unsigned char main_third_edge);
        //void verify_cubes_face(cube *c_parent, unsigned short neighbor_cube, unsigned short neighbor_cube_face, unsigned short main_cube, unsigned short main_cube_face);
        void verify_cubes_face(cube *neighbor_c, cube *main_c, unsigned short main_cube_face);

        //code for intersection with octree cubes
        glm::vec3 ray_octree_intersection(glm::vec3 position, glm::vec3 direction);
        bool process_octree_children( float t0x, float t0y, float t0z, float t1x, float t1y, float t1z, cube *c, glm::vec3 &inter_point, unsigned char a);
        unsigned short find_first_cube(float t0x, float t0y, float t0z, float tMx, float tMy, float tMz);
        unsigned short find_next_cube(float x, float y, float z, unsigned short a, unsigned short b, unsigned short c);
        //end

        //void findMousePointer(glm::vec3 position, glm::vec3 direction);
        void update_level(short mouse_scroll);
        void SetupOctree(GLuint shader);

        void generate_buffer_objects();

        data_selection generate_cube_vertices(glm::vec3 center, float radius, bool cube_selection);

        void KeyGPressed(bool condition);
        bool ReturnKeyG();

        void MarkSelection();
        void UnmarkSelection();
        bool IsSelected();

        void store_data_to_gpu(data_selection cube_data);
        void Draw(GLuint shader, MVPuniforms unifrom, MVPmatrices matrix);
        void render_selection_cubes(GLuint shader, MVPuniforms uniform, MVPmatrices matrix);

        //render map functions

        void MapGeneration(cube *map_data);
        void traverse_octree_data(cube *octree_data, glm::vec3 center, float radius, vector<vector_data> &map_data_vertices);
        glm::vec3 calculate_octree_center(glm::vec3 center, float radius, unsigned short i);
        cube_vertices generate_cube_final_vertices(cube *c, glm::vec3 center, float radius);
        float calculate_axis(char axis, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
        cube_vertices calculate_cube_vertices(glm::vec3 center, float radius);
        void create_vector_map(cube_vertices Vert, vector<vector_data> &map_data, cube *c);
        void generate_cube_face(vector_data &CubeFaces, cube_vertices Vert, cube *c, unsigned short face_index, unsigned short luc_vertex, unsigned short lbc_vertex, unsigned short ruc_vertex, unsigned short rbc_vertex);

        void generate_buffer_objects2();
        void store_data_to_gpu2(vector<vector_data> map_data);
        void Render(GLuint shader, MVPuniforms uniform, MVPmatrices matrix);

        void SetupOctree2(GLuint shader);
};

