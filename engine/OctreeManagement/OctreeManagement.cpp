#include "OctreeManagement.hpp"

// editing mode functions

cube *OctreeManagement::new_cubes(bool consistency) {
    cube *c = new cube[8];
    for (unsigned short i = 0; i < 8; i++)
    {
        c[i].children = 0;
        set_edge_spans(c[i]);
        set_all_faces_to_false(c[i]);
        c[i].solid = consistency;
        //more code to follow...
        //c++;
    }
    //return c-8;
    return c;
}

bool OctreeManagement::subdivide_cube(cube &c) {
    if(c.children) {return true;}
    c.children = new_cubes(c.solid);
    //set visible faces for all children based on their parent
    set_octree_face_visibility(&c);

    return true;
}

void OctreeManagement::set_edge_spans(cube &c) {
    for (unsigned short i = 0; i < 12; i++)
    {
        c.edges[i] = {0, 8};
    }
}

void OctreeManagement::set_all_faces_to_false(cube &c) {
    set_cube_face_visibility(c, 0, false);
    set_cube_face_visibility(c, 1, false);
    set_cube_face_visibility(c, 2, false);
    set_cube_face_visibility(c, 3, false);
    set_cube_face_visibility(c, 4, false);
    set_cube_face_visibility(c, 5, false);
}

void OctreeManagement::set_octree_face_visibility(cube *c) {
    //verify face 0/positive x of the parent
    set_children_face_visibility(c, 0, 0, 4, 3, 7);

    //verify face 1/negative x of the parent
    set_children_face_visibility(c, 1, 2, 6, 1, 5);

    //verify face 2/positive y of the parent
    set_children_face_visibility(c, 2, 2, 1, 3, 0);

    //verify face 3/negative y of the parent
    set_children_face_visibility(c, 3, 5, 6, 4, 7);

    //verify face 4/positive z of the parent
    set_children_face_visibility(c, 4, 1, 5, 0, 4);

    //verify face 5/negative z of the parent
    set_children_face_visibility(c, 5, 3, 7, 2, 6);
}

void OctreeManagement::set_cube_face_visibility(cube &c, unsigned short face_index, bool visibility) {
    c.face[face_index].lbc = c.face[face_index].luc = c.face[face_index].rbc = c.face[face_index].ruc = visibility;
}

void OctreeManagement::set_children_face_visibility(cube *c, unsigned short face_index, unsigned short luc_cube, unsigned short lbc_cube, unsigned short ruc_cube, unsigned short rbc_cube) {
    if (c->face[face_index].lbc && c->face[face_index].luc ||
        c->face[face_index].lbc && c->face[face_index].rbc ||
        c->face[face_index].lbc && c->face[face_index].ruc ||
        c->face[face_index].luc && c->face[face_index].rbc ||
        c->face[face_index].luc && c->face[face_index].ruc ||
        c->face[face_index].rbc && c->face[face_index].ruc)
    {
        set_cube_face_visibility(c->children[luc_cube], face_index, true);
        set_cube_face_visibility(c->children[ruc_cube], face_index, true);
        set_cube_face_visibility(c->children[lbc_cube], face_index, true);
        set_cube_face_visibility(c->children[rbc_cube], face_index, true);
    }
    else if (c->face[face_index].lbc)
    {
        set_cube_face_visibility(c->children[lbc_cube], face_index, true);
        c->children[luc_cube].face[face_index].lbc = true;
        c->children[rbc_cube].face[face_index].lbc = true;
        //ruc_cube should be invisible and the remaining corners of luc_cube an rbc_cube as well
    }
    else if (c->face[face_index].ruc)
    {
        set_cube_face_visibility(c->children[ruc_cube], face_index, true);
        c->children[luc_cube].face[face_index].ruc = true;
        c->children[rbc_cube].face[face_index].ruc = true;
    }
    else if (c->face[face_index].rbc)
    {
        set_cube_face_visibility(c->children[rbc_cube], face_index, true);
        c->children[ruc_cube].face[face_index].rbc = true;
        c->children[lbc_cube].face[face_index].rbc = true;
    }
    else if(c->face[face_index].luc)
    {
        set_cube_face_visibility(c->children[luc_cube], face_index, true);
        c->children[ruc_cube].face[face_index].luc = true;
        c->children[lbc_cube].face[face_index].luc = true;
    }
}

void OctreeManagement::GenerateOctreeData(short mouse_scroll) {
    unsigned short child_cube;

    if (mouse_scroll == 1)
    {
        //translate the center of the selected cube along the orientation
        translate_cube_center(1);
        //return the octree cube pointed by cube_selection
        cube *c_parent = return_selected_cube(child_cube, cube_selection.center);
        //delete all of the children of the pointed cube
        delete_children(c_parent->children[child_cube]);
        c_parent->children[child_cube].solid = true;
        //update face visibility of the modified cube and it's neighbor cubes
        //update_cube_face_visibility(c_parent, child_cube, 1);
        cube_selection = generate_cube_vertices(cube_selection.center, cube_selection.radius, true);
    }
    else if (mouse_scroll == -1)
    {
        //return the octree cube pointed by cube_selection
        cube *c_parent = return_selected_cube(child_cube, cube_selection.center);
        //delete all of the children of the pointed cube
        delete_children(c_parent->children[child_cube]);
        c_parent->children[child_cube].solid = false;
        //update face visibility of the modified cube and it's neighbor cubes
        //update_cube_face_visibility(c_parent, child_cube, -1);
        //translate the center of the selected cube along the orientation
        translate_cube_center(-1);
        cube_selection = generate_cube_vertices(cube_selection.center, cube_selection.radius, true);
    }
}

void OctreeManagement::function_new_function(cube *c_parent) {
    /////////////////////////////////////////////////////////////
    //verify each corner of cube 0 face[1]
    //first corner
    if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
    {
        if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
        {
            c_parent->children[1].face[0].luc = false;
            c_parent->children[0].face[1].ruc = true;
        }
        else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = true;
        }
        else
        {
            c_parent->children[1].face[0].luc = false;
            c_parent->children[0].face[1].ruc = true;
        }
    }
    else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
    {
        if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = true;
        }
        else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = false;
        }
        else
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = false;
        }
    }
    else if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
    {
        if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
        {
            c_parent->children[1].face[0].luc = false;
            c_parent->children[0].face[1].ruc = true;
        }
        else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = true;
        }
        else
        {
            c_parent->children[1].face[0].luc = false;
            c_parent->children[0].face[1].ruc = true;
        }
    }
    else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
    {
        if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = true;
        }
        else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = false;
        }
        else
        {
            c_parent->children[1].face[0].luc = true;
            c_parent->children[0].face[1].ruc = false;
        }
    }
    else
    {
        c_parent->children[1].face[0].luc = false;
        c_parent->children[0].face[1].ruc = false;
    }
    ////////////////////////////////////////////////////////////
}
/*
void OctreeManagement::update_cube_face_visibility(cube *c_parent, unsigned short child_cube, short mouse_scroll) {
    float neighbor_point;
    cube *neighbor_cube;
    unsigned short i;

    switch (mouse_scroll)
    {
        case 1:
            switch (child_cube)
            {
                case 0:
                    //face X
                    // find neighbor cube
                    neighbor_point = cube_selection.center.x + cube_selection.radius + 0.06;
                    neighbor_cube = return_neighbor_cube(i, neighbor_point);

                    if (neighbor_cube.solid)
                    {
                        face
                    }

                    break;

                case 1:

                    break;

                case 2:

                    break:

                case 3:

                    break;

                case 4:

                    break;

                case 5:

                    break;

                case 6:

                    break;

                case 7:

                    break;
                /*case 0:
                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        /////////////////////////////////////////////////////////////
                        //verify each corner of cube 0 fase[1]
                        //first corner
                        if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
                        {
                            if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
                            {
                                c_parent->children[1].face[0].luc = false;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else
                            {
                                c_parent->children[1].face[0].luc = false;
                                c_parent->children[0].face[1].ruc = true;
                            }
                        }
                        else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
                        {
                            if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = false;
                            }
                            else
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = false;
                            }
                        }
                        else if (c_parent->children[1].edges[9].ending < c_parent->children[0].edges[8].ending)
                        {
                            if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
                            {
                                c_parent->children[1].face[0].luc = false;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else
                            {
                                c_parent->children[1].face[0].luc = false;
                                c_parent->children[0].face[1].ruc = true;
                            }
                        }
                        else if (c_parent->children[1].edges[9].ending > c_parent->children[0].edges[8].ending)
                        {
                            if (c_parent->children[1].edges[7].ending < c_parent->children[0].edges[6].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = true;
                            }
                            else if (c_parent->children[1].edges[7].ending > c_parent->children[0].edges[6].ending)
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = false;
                            }
                            else
                            {
                                c_parent->children[1].face[0].luc = true;
                                c_parent->children[0].face[1].ruc = false;
                            }
                        }
                        else
                        {
                            c_parent->children[1].face[0].luc = false;
                            c_parent->children[0].face[1].ruc = false;
                        }
                        ////////////////////////////////////////////////////////////
                    }
                    else
                    {
                        c_parent->children[0].face[1].ruc = true;
                    }

                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[0].face[3] = true;
                    }

                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[0].face[5] = true;
                    }
                    break;

                case 1:
                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[0] = true;
                    }

                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[3] = true;
                    }

                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[5] = true;
                    }
                    break;

                case 2:
                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[0] = true;
                    }

                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[3] = true;
                    }

                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        c_parent->children[1].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[4] = true;
                    }
                    break;

                case 3:
                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[1] = true;
                    }

                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[3] = true;
                    }

                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[4] = true;
                    }
                    break;

                case 4:
                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[1] = true;
                    }

                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[2] = true;
                    }

                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[5] = true;
                    }
                    break;

                case 5:
                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[0] = true;
                    }

                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        c_parent->children[1].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[2] = true;
                    }

                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[5] = true;
                    }
                    break;

                case 6:
                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[0] = true;
                    }

                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[2] = true;
                    }

                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[4] = true;
                    }
                    break;

                case 7:
                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[1] = true;
                    }

                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[2] = true;
                    }

                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[4] = true;
                    }
                    break;*/
/*            }
            break;

        case -1:
            switch (child_cube)
            {
                case 0:
                    //face X
                    // find neighbor cube
                    neighbor_point = cube_selection.center.x + cube_selection.radius + 0.06;
                    neighbor_cube = return_neighbor_cube(i, neighbor_point);

                    if (neighbor_cube.solid)
                    {
                        face
                    }

                    break;

                case 1:

                    break;

                case 2:

                    break:

                case 3:

                    break;

                case 4:

                    break;

                case 5:

                    break;

                case 6:

                    break;

                case 7:

                    break;
            }
            break;

/*        case -1:
            switch (child_cube)
            {
                case 0:
                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        c_parent->children[1].face[0] = true;
                        c_parent->children[0].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[0].face[1] = false;
                    }

                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[2] = true;
                        c_parent->children[0].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[0].face[3] = false;
                    }

                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[4] = true;
                        c_parent->children[0].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[0].face[5] = false;
                    }
                    break;

                case 1:
                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[1] = true;
                        c_parent->children[1].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[0] = false;
                    }

                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[2] = true;
                        c_parent->children[1].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[3] = false;
                    }

                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[4] = true;
                        c_parent->children[1].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[1].face[5] = false;
                    }
                    break;

                case 2:
                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[1] = true;
                        c_parent->children[2].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[0] = false;
                    }

                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[2] = true;
                        c_parent->children[2].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[3] = false;
                    }

                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        c_parent->children[1].face[5] = true;
                        c_parent->children[2].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[2].face[4] = false;
                    }
                    break;

                case 3:
                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[0] = true;
                        c_parent->children[3].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[1] = false;
                    }

                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[2] = true;
                        c_parent->children[3].face[3] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[3] = false;
                    }

                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[5] = true;
                        c_parent->children[3].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[3].face[4] = false;
                    }
                    break;

                case 4:
                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[0] = true;
                        c_parent->children[4].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[1] = false;
                    }

                    //cube 0
                    if (c_parent->children[0].solid)
                    {
                        c_parent->children[0].face[3] = true;
                        c_parent->children[4].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[2] = false;
                    }

                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[4] = true;
                        c_parent->children[4].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[4].face[5] = false;
                    }
                    break;

                case 5:
                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[1] = true;
                        c_parent->children[5].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[0] = false;
                    }

                    //cube 1
                    if (c_parent->children[1].solid)
                    {
                        c_parent->children[1].face[3] = true;
                        c_parent->children[5].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[2] = false;
                    }

                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[4] = true;
                        c_parent->children[5].face[5] = false;
                    }
                    else
                    {
                        c_parent->children[5].face[5] = false;
                    }
                    break;

                case 6:
                    //cube 7
                    if (c_parent->children[7].solid)
                    {
                        c_parent->children[7].face[1] = true;
                        c_parent->children[6].face[0] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[0] = false;
                    }

                    //cube 2
                    if (c_parent->children[2].solid)
                    {
                        c_parent->children[2].face[3] = true;
                        c_parent->children[6].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[2] = false;
                    }

                    //cube 5
                    if (c_parent->children[5].solid)
                    {
                        c_parent->children[5].face[5] = true;
                        c_parent->children[6].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[6].face[4] = false;
                    }
                    break;

                case 7:
                    //cube 6
                    if (c_parent->children[6].solid)
                    {
                        c_parent->children[6].face[0] = true;
                        c_parent->children[7].face[1] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[1] = false;
                    }

                    //cube 3
                    if (c_parent->children[3].solid)
                    {
                        c_parent->children[3].face[3] = true;
                        c_parent->children[7].face[2] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[2] = false;
                    }

                    //cube 4
                    if (c_parent->children[4].solid)
                    {
                        c_parent->children[4].face[5] = true;
                        c_parent->children[7].face[4] = false;
                    }
                    else
                    {
                        c_parent->children[7].face[4] = false;
                    }
                    break;
            }
            break;
    }
}*/


cube *OctreeManagement::return_selected_cube(unsigned short &child_cube, glm::vec3 cube_location) {
    //move along the Octree to the pointed cube
    float new_radius = world_radius;
    glm::vec3 new_center = world_center;
    cube *c_parent = worldroot;
    child_cube = find_child_cube(cube_location, new_center);

    cube *c = &worldroot->children[child_cube];
    new_center = findOctreeCenter(cube_location, new_center, new_radius);
    new_radius = new_radius/2;

    for(unsigned short i = 1; i < level; i++)
    {
        subdivide_cube(*c);
        c_parent = c;
        child_cube = find_child_cube(cube_location, new_center);
        c = &c->children[child_cube];
        new_center = findOctreeCenter(cube_location, new_center, new_radius);
        new_radius = new_radius/2;
    }
    return c_parent;
}

///////////////////////////////////////////////////////////////////
//return neighbor cube
cube *OctreeManagement::return_neighbor_cube(unsigned short i, glm::vec3 cube_location) {
    glm::vec3 center = world_center;
    float radius = world_radius;

    cube *c = &worldroot->children[find_child_cube(cube_location, center)];
    center = findOctreeCenter(cube_location, center, radius);
    radius = radius/2;

    i = 1;

    if (c->children && i < level) do
    {
        i++;
        c = &c->children[find_child_cube(cube_location, center)];
        center = findOctreeCenter(cube_location, center, radius);
        radius = radius/2;
    } while (c->children && i < level);

    return c;
}
///////////////////////////////////////////////////////////////////

void OctreeManagement::delete_children(cube &c) {
    if (c.children)
    {
        for (unsigned short i = 0; i < 8; i++) delete_children(c.children[i]);
        delete[] c.children;
        c.children = 0;
    }

}

void OctreeManagement::translate_cube_center(short mouse_scroll) {
 //translate the center of the selected cube along the orientation based on mouse_scroll
 switch (mouse_scroll)
 {
  case 1:
   switch (selection_orientation)
   {
    case 'X':
     cube_selection.center.x += 2 * cube_selection.radius;
     if (cube_selection.center.x > (world_center.x + world_radius - cube_selection.radius))
      cube_selection.center.x = world_center.x + world_radius - cube_selection.radius;
     break;

    case 'x':
     cube_selection.center.x -= 2 * cube_selection.radius;
     if (cube_selection.center.x < (world_center.x - world_radius + cube_selection.radius))
      cube_selection.center.x = world_center.x - world_radius + cube_selection.radius;
     break;

    case 'Y':
     cube_selection.center.y += 2 * cube_selection.radius;
     if (cube_selection.center.y > (world_center.y + world_radius - cube_selection.radius))
      cube_selection.center.y = world_center.y + world_radius - cube_selection.radius;
     break;

    case 'y':
     cube_selection.center.y -= 2 * cube_selection.radius;
     if (cube_selection.center.y < (world_center.y - world_radius + cube_selection.radius))
      cube_selection.center.y = world_center.y - world_radius + cube_selection.radius;
     break;

    case 'Z':
     cube_selection.center.z += 2 * cube_selection.radius;
     if (cube_selection.center.z > (world_center.z + world_radius - cube_selection.radius))
      cube_selection.center.z = world_center.z + world_radius - cube_selection.radius;
     break;

    case 'z':
     cube_selection.center.z -= 2 * cube_selection.radius;
     if (cube_selection.center.z < (world_center.z - world_radius + cube_selection.radius))
      cube_selection.center.z = world_center.z - world_radius + cube_selection.radius;
     break;
   }
   break;

  case -1:
   switch (selection_orientation)
   {
    case 'X':
     cube_selection.center.x -= 2 * cube_selection.radius;
     if (cube_selection.center.x < (world_center.x - world_radius - cube_selection.radius))
      cube_selection.center.x = world_center.x - world_radius - cube_selection.radius;
     break;

    case 'x':
     cube_selection.center.x += 2 * cube_selection.radius;
     if (cube_selection.center.x > (world_center.x + world_radius + cube_selection.radius))
      cube_selection.center.x = world_center.x + world_radius + cube_selection.radius;
     break;

    case 'Y':
     cube_selection.center.y -= 2 * cube_selection.radius;
     if (cube_selection.center.y < (world_center.y - world_radius - cube_selection.radius))
      cube_selection.center.y = world_center.y - world_radius - cube_selection.radius;
     break;

    case 'y':
     cube_selection.center.y += 2 * cube_selection.radius;
     if (cube_selection.center.y > (world_center.y + world_radius + cube_selection.radius))
      cube_selection.center.y = world_center.y + world_radius + cube_selection.radius;
     break;

    case 'Z':
     cube_selection.center.z -= 2 * cube_selection.radius;
     if (cube_selection.center.z < (world_center.z - world_radius - cube_selection.radius))
      cube_selection.center.z = world_center.z - world_radius - cube_selection.radius;
     break;

    case 'z':
     cube_selection.center.z += 2 * cube_selection.radius;
     if (cube_selection.center.z > (world_center.z + world_radius + cube_selection.radius))
      cube_selection.center.z = world_center.z + world_radius + cube_selection.radius;
     break;
   }
   break;
 }
 //end
}

unsigned short OctreeManagement::find_child_cube(glm::vec3 pointer_position, glm::vec3 center) {
    if(pointer_position.x >= center.x)
    {
        if(pointer_position.y >= center.y)
        {
            if(pointer_position.z >= center.z)
            {
                //cube 0
                return 0;
            }
            else if (pointer_position.z < center.z)
            {
                //cube 3
                return 3;
            }
        }
        else if (pointer_position.y < center.y)
        {
            if(pointer_position.z >= center.z)
            {
                //cube 4
                return 4;
            }
            else if (pointer_position.z < center.z)
            {
                //cube 7
                return 7;
            }
        }
    }
    else if (pointer_position.x < center.x)
    {
        if(pointer_position.y >= center.y)
        {
            if(pointer_position.z >= center.z)
            {
                //cube 1
                return 1;
            }
            else if (pointer_position.z < center.z)
            {
                //cube 2
                return 2;
            }
        }
        else if (pointer_position.y < center.y)
        {
            if(pointer_position.z >= center.z)
            {
                //cube 5
                return 5;
            }
            else if (pointer_position.z < center.z)
            {
                //cube 6
                return 6;
            }
        }
    }
}

void OctreeManagement::generate_octree() {
    worldroot = new cube;
    worldroot->children = new_cubes(true);
    worldroot->children[0].solid = false;
    worldroot->children[1].solid = false;
    worldroot->children[2].solid = false;
    worldroot->children[3].solid = false;
    //worldroot[4].solid = false;
    //worldroot->children[5].solid = false;
    //worldroot->children[6].solid = false;
    //worldroot->children[7].solid = false;
    set_cube_face_visibility(worldroot->children[4], 0, true);
    set_cube_face_visibility(worldroot->children[4], 2, true);
    set_cube_face_visibility(worldroot->children[4], 3, true);
    set_cube_face_visibility(worldroot->children[4], 4, true);

    set_cube_face_visibility(worldroot->children[5], 1, true);
    set_cube_face_visibility(worldroot->children[5], 2, true);
    set_cube_face_visibility(worldroot->children[5], 3, true);
    set_cube_face_visibility(worldroot->children[5], 4, true);

    set_cube_face_visibility(worldroot->children[6], 1, true);
    set_cube_face_visibility(worldroot->children[6], 2, true);
    set_cube_face_visibility(worldroot->children[6], 3, true);
    set_cube_face_visibility(worldroot->children[6], 5, true);

    set_cube_face_visibility(worldroot->children[7], 0, true);
    set_cube_face_visibility(worldroot->children[7], 2, true);
    set_cube_face_visibility(worldroot->children[7], 3, true);
    set_cube_face_visibility(worldroot->children[7], 5, true);


/*    worldroot = new_cubes(true);
    worldroot[0].solid = false;
    worldroot[1].solid = false;
    worldroot[2].solid = false;
    worldroot[3].solid = false;
    //worldroot[4].solid = false;
    worldroot[5].solid = false;
    worldroot[6].solid = false;
    worldroot[7].solid = false; */
    world_center = glm::vec3(512.0f, 512.0f, 512.0f);
    world_radius = 512.0f;
    //world_center = glm::vec3(204.8f, 204.8f, 204.8f);
    //world_radius = 204.8;
    level = 1;
    orientation = 'X';

    keyG = false;

    on_click = false;
    selected = false;
}

cube *OctreeManagement::return_worldroot() {
    return worldroot;
}

/*
glm::vec3 OctreeManagement::return_intersection_point(glm::vec3 position, glm::vec3 direction) {

    glm::vec3 inter_point;

    glm::vec3 point = position;// + (0.01f * glm::normalize(direction));

    while (point.x < world_center.x + world_radius && point.x > world_center.x - world_radius &&
            point.y < world_center.y + world_radius && point.y > world_center.y - world_radius &&
            point.z < world_center.z + world_radius && point.z > world_center.z - world_radius)
    {
        cube_center = world_center;
        cube_radius = world_radius;

        cube *c = &worldroot[find_child_cube(point, cube_center)];
        cube_center = findOctreeCenter(point, cube_center, cube_radius);
        cube_radius = cube_radius/2;

        unsigned short i = 1;

        if (c->children && i < level) do
        {
            i++;
            c = &c->children[find_child_cube(point, cube_center)];
            cube_center = findOctreeCenter(point, cube_center, cube_radius);
            cube_radius = cube_radius/2;
        } while (c->children && i < level);

        //advance the point along the raycast
        //float n = world_radius/pow(2, level);
        //cerr << n<<endl;
        point = point + (glm::normalize(direction));
        //cerr<<point.x<<endl;

        if (c->children)
        {
            if (is_octree_parent_solid(c->children, cube_center, cube_radius))
            {
                //inter_point = cube_center;
                inter_point = line_cube_intersection(cube_center, cube_radius, position, direction);
                //line_cube_intersection(cube_center, cube_radius, position, direction);
                return inter_point;
            }
            else
            {
                continue;
            }
        }
        if (!(c->solid)) continue;
        //verify if it has children too and do something ...
        //make vertices from cube c, find 6 faces, intersect each face with the ray and find the intersection point
        //inter_point = cube_center;
        inter_point = line_cube_intersection(cube_center, cube_radius, position, direction);
        //line_cube_intersection(cube_center, cube_radius, position, direction);
        return inter_point;

    }
    //return inter_point;
    //cerr<<"hello"<<endl;
}


glm::vec3 OctreeManagement::return_intersection_point2(glm::vec3 position, glm::vec3 direction, GLushort window_width, GLushort window_height, MVPmatrices matrix) {
    GLfloat depth;
    //glReadPixels(window_width/2, window_height/2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    //cerr<<"depth\t"<<depth<<endl;
    //if (depth == 1) return glm::vec3(0, 0, 0);
    glm::vec4 viewport = glm::vec4(0, 0, window_width, window_height);
    glm::vec3 wincoord = glm::vec3(window_width/2, window_height/2, depth);
    glm::vec3 objcoord = glm::unProject(wincoord, matrix.View, matrix.Projection, viewport);
    //cerr<<"obj.x\t"<<objcoord.x<<endl;
    //cerr<<"obj.y\t"<<objcoord.y<<endl;
    //cerr<<"obj.z\t"<<objcoord.z<<endl;
}

bool OctreeManagement::is_octree_parent_solid(cube *octree_data, glm::vec3 center, float radius) {

    bool condition = false;

    for (unsigned short i = 0; i < 8; i++)
    {
        float new_radius;
        glm::vec3 new_center;
        cube *c = &octree_data[i];

        new_center = calculate_octree_center(center, radius, i);
        new_radius = radius/2;

        if (c->children)
        {
            condition = is_octree_parent_solid(c->children, new_center, new_radius);
        }
        else
        {
            if ((c->solid)) return true;
        }

        if (condition) return true;
    }
    return condition;
}

glm::vec3 OctreeManagement::line_cube_intersection(glm::vec3 center, float radius, glm::vec3 position, glm::vec3 direction) {

    cube_vertices Vert = calculate_cube_vertices(center, radius);

    face_axis Face = find_visible_faces(center, position, Vert);



    glm::vec3 inter_point;
    glm::vec3 normal;
    float distance;

    float n = - world_radius/pow(2, level);

    //face x
    normal = glm::normalize(glm::cross(Face.X.a - Face.X.b, Face.X.c - Face.X.b));
    distance = glm::dot(normal, (position - Face.X.b));
    if (distance > 0)
    {
        //cerr<<"X\t"<<distance<<endl;
        inter_point = line_plane_intersection(position, direction, normal, Face.X.b);
        if (Face.X.orient == 'X')
        {
            if (inter_point.y > Face.X.c.y && inter_point.y < Face.X.b.y && inter_point.z > Face.X.b.z && inter_point.z < Face.X.a.z)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
        else
        {
            if (inter_point.y > Face.X.c.y && inter_point.y < Face.X.b.y && inter_point.z > Face.X.a.z && inter_point.z < Face.X.b.z)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
    }

    //face y
    normal = glm::normalize(glm::cross(Face.Y.a - Face.Y.b, Face.Y.c - Face.Y.b));
    distance = glm::dot(normal, (position - Face.Y.b));
    if (distance > 0)
    {
        //cerr<<"Y\t"<<distance<<endl;
        inter_point = line_plane_intersection(position, direction, normal, Face.Y.b);
        if (Face.Y.orient == 'Y')
        {
            if (inter_point.x > Face.Y.a.x && inter_point.x < Face.Y.b.x && inter_point.z > Face.Y.b.z && inter_point.z < Face.Y.c.z)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
        else
        {
            if (inter_point.x > Face.Y.a.x && inter_point.x < Face.Y.b.x && inter_point.z > Face.Y.c.z && inter_point.z < Face.Y.b.z)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
    }

    //face z
    normal = glm::normalize(glm::cross(Face.Z.a - Face.Z.b, Face.Z.c - Face.Z.b));
    distance = glm::dot(normal, (position - Face.Z.b));
    if (distance > 0)
    {
        //cerr<<"Z\t"<<distance<<endl;
        inter_point = line_plane_intersection(position, direction, normal, Face.Z.b);
        if (Face.Z.orient == 'Z')
        {
            if (inter_point.x > Face.Z.a.x && inter_point.x < Face.Z.b.x && inter_point.y > Face.Z.c.y && inter_point.y < Face.Z.b.y)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
        else
        {
            if (inter_point.x > Face.Z.b.x && inter_point.x < Face.Z.a.x && inter_point.y > Face.Z.c.y && inter_point.y < Face.Z.b.y)
            {
                calculate_orientation(inter_point, center, radius);
                inter_point = inter_point + (n * normal);
                return inter_point;
            }
        }
    }
    return inter_point;
}

glm::vec3 OctreeManagement::line_plane_intersection(glm::vec3 origin, glm::vec3 direction, glm::vec3 normal, glm::vec3 point_on_plane) {
    glm::vec3 inter_point;
    float number = glm::dot(normal, direction);
    //cerr<<"number\t"<<number<<endl;
    if (number == 0) return inter_point = glm::vec3(0.0f, 0.0f, 0.0f);
    inter_point = origin + direction * (glm::dot(-normal, (origin - point_on_plane)/number));
    return inter_point;
}

void OctreeManagement::calculate_orientation(glm::vec3 point, glm::vec3 center, float radius) {
    if (point.x == center.x + radius)
    {
        orientation = 'X';
    }
    else if (point.x == center.x - radius)
    {
        orientation = 'x';
    }

    if (point.y == center.y + radius)
    {
        orientation = 'Y';
    }
    else if (point.y == center.y - radius)
    {
        orientation = 'y';
    }

    if (point.z == center.z + radius)
    {
        orientation = 'Z';
    }
    else if (point.z == center.z - radius)
    {
        orientation = 'z';
    }
}

face_axis OctreeManagement::find_visible_faces(glm::vec3 center, glm::vec3 position, cube_vertices Vert) {

    face_axis Face;

    static float_edge_span edges[3];

    if (position.x > center.x)
    {
        if (position.y > center.y)
        {
            if (position.z > center.z)
            {
                //face X
                //face Y
                //face Z
                Face.X.orient = 'X';
                Face.X.a = Vert.vertices[0];
                Face.X.b = Vert.vertices[3];
                Face.X.c = Vert.vertices[7];

                Face.Y.orient = 'Y';
                Face.Y.a = Vert.vertices[2];
                Face.Y.b = Vert.vertices[3];
                Face.Y.c = Vert.vertices[0];

                Face.Z.orient = 'Z';
                Face.Z.a = Vert.vertices[1];
                Face.Z.b = Vert.vertices[0];
                Face.Z.c = Vert.vertices[4];
            }
            else
            {
                //face X
                //face Y
                //face z
                Face.X.orient = 'X';
                Face.X.a = Vert.vertices[0];
                Face.X.b = Vert.vertices[3];
                Face.X.c = Vert.vertices[7];

                Face.Y.orient = 'Y';
                Face.Y.a = Vert.vertices[2];
                Face.Y.b = Vert.vertices[3];
                Face.Y.c = Vert.vertices[0];

                Face.Z.orient = 'z';
                Face.Z.a = Vert.vertices[3];
                Face.Z.b = Vert.vertices[2];
                Face.Z.c = Vert.vertices[6];
            }
        }
        else
        {
           if (position.z > center.z)
           {
                //face X
                //face y
                //face Z
                Face.X.orient = 'X';
                Face.X.a = Vert.vertices[0];
                Face.X.b = Vert.vertices[3];
                Face.X.c = Vert.vertices[7];

                Face.Y.orient = 'y';
                Face.Y.a = Vert.vertices[5];
                Face.Y.b = Vert.vertices[4];
                Face.Y.c = Vert.vertices[7];

                Face.Z.orient = 'Z';
                Face.Z.a = Vert.vertices[1];
                Face.Z.b = Vert.vertices[0];
                Face.Z.c = Vert.vertices[4];
           }
           else
           {
                //face X
                //face y
                //face z
                Face.X.orient = 'X';
                Face.X.a = Vert.vertices[0];
                Face.X.b = Vert.vertices[3];
                Face.X.c = Vert.vertices[7];

                Face.Y.orient = 'y';
                Face.Y.a = Vert.vertices[5];
                Face.Y.b = Vert.vertices[4];
                Face.Y.c = Vert.vertices[7];

                Face.Z.orient = 'z';
                Face.Z.a = Vert.vertices[3];
                Face.Z.b = Vert.vertices[2];
                Face.Z.c = Vert.vertices[6];
           }
        }
    }
    else
    {
        if (position.y > center.y)
        {
            if (position.z > center.z)
            {
                //face x
                //face Y
                //face Z
                Face.X.orient = 'x';
                Face.X.a = Vert.vertices[2];
                Face.X.b = Vert.vertices[1];
                Face.X.c = Vert.vertices[5];

                Face.Y.orient = 'Y';
                Face.Y.a = Vert.vertices[2];
                Face.Y.b = Vert.vertices[3];
                Face.Y.c = Vert.vertices[0];

                Face.Z.orient = 'Z';
                Face.Z.a = Vert.vertices[1];
                Face.Z.b = Vert.vertices[0];
                Face.Z.c = Vert.vertices[4];
            }
            else
            {
                //face x
                //face Y
                //face z
                Face.X.orient = 'x';
                Face.X.a = Vert.vertices[2];
                Face.X.b = Vert.vertices[1];
                Face.X.c = Vert.vertices[5];

                Face.Y.orient = 'Y';
                Face.Y.a = Vert.vertices[2];
                Face.Y.b = Vert.vertices[3];
                Face.Y.c = Vert.vertices[0];

                Face.Z.orient = 'z';
                Face.Z.a = Vert.vertices[3];
                Face.Z.b = Vert.vertices[2];
                Face.Z.c = Vert.vertices[6];
            }
        }
        else
        {
            if (position.z > center.z)
            {
                //face x
                //face y
                //face Z
                Face.X.orient = 'x';
                Face.X.a = Vert.vertices[2];
                Face.X.b = Vert.vertices[1];
                Face.X.c = Vert.vertices[5];

                Face.Y.orient = 'y';
                Face.Y.a = Vert.vertices[5];
                Face.Y.b = Vert.vertices[4];
                Face.Y.c = Vert.vertices[7];

                Face.Z.orient = 'Z';
                Face.Z.a = Vert.vertices[1];
                Face.Z.b = Vert.vertices[0];
                Face.Z.c = Vert.vertices[4];
            }
            else
            {
                //face x
                //face y
                //face z
                Face.X.orient = 'x';
                Face.X.a = Vert.vertices[2];
                Face.X.b = Vert.vertices[1];
                Face.X.c = Vert.vertices[5];

                Face.Y.orient = 'y';
                Face.Y.a = Vert.vertices[5];
                Face.Y.b = Vert.vertices[4];
                Face.Y.c = Vert.vertices[7];

                Face.Z.orient = 'z';
                Face.Z.a = Vert.vertices[3];
                Face.Z.b = Vert.vertices[2];
                Face.Z.c = Vert.vertices[6];
            }
        }
    }
    return Face;
}
*/

glm::vec3 OctreeManagement::ray_octree_intersection(glm::vec3 position, glm::vec3 direction) {
    glm::vec3 center = world_center;
    float radius = world_radius;

    glm::vec3 inter_point = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 t_values(0.0f, 0.0f, 0.0f);
    glm::vec3 position_default = position;
    glm::vec3 first_direction = direction;

    short x = 1;
    short y = 1;
    short z = 1;

    //vertex 6
    //glm::vec3 min_point = glm::vec3(center.x - radius, center.y - radius, center.z - radius);
    //glm::vec3 min_point = glm::vec3(512, 0, 512);
    glm::vec3 min_point = glm::vec3(0.0f, 0.0f, 0.0f);
    //vertex 0
    //glm::vec3 max_point = glm::vec3(center.x + radius, center.y, center.z + radius);
    glm::vec3 max_point = glm::vec3(center.x + radius, center.y + radius, center.z + radius);
    //glm::vec3 max_point = glm::vec3(0, 0, 0);

    unsigned char a = 6;
    if (direction.x < 0)
    {
        position.x = 2 * min_point.x + 2 * radius  - position.x;
        direction.x = -(direction.x);
        a ^= 1;//a = 7
        x = -1;
    }
    if (direction.y < 0)
    {
        position.y = 2 * min_point.y + 2 * radius  - position.y;
        direction.y = -(direction.y);
        a ^= 4;//a = 2
        y = -1;
    }
    if (direction.z < 0)
    {
        position.z = 2 * min_point.z + 2 * radius - position.z;
        direction.z = -(direction.z);
        a ^= 3;//a = 5
        z = -1;
    }

    float t0x = (min_point.x - position.x)/direction.x;
    float t0y = (min_point.y - position.y)/direction.y;
    float t0z = (min_point.z - position.z)/direction.z;

    float t1x = (max_point.x - position.x)/direction.x;
    float t1y = (max_point.y - position.y)/direction.y;
    float t1z = (max_point.z - position.z)/direction.z;

    float tmin = max(max(t0x, t0y), t0z);
    float tmax = min(min(t1x, t1y), t1z);

    //cerr<<"tmin\t"<<tmin<<endl;
    //cerr<<"tmax\t"<<tmax<<endl<<endl;

    //cerr<<"first_direction.z\t"<<first_direction.z<<endl;
    //cerr<<"direction.z\t"<<direction.z<<endl;
    //cerr<<"position.z\t"<<position.z<<endl<<endl;

    if ((tmin < tmax) && (tmax > 0.0f))
    {
        process_octree_children(t0x, t0y, t0z, t1x, t1y, t1z, worldroot, t_values, a);
    }

    float max_t = max(max(t_values.x, t_values.y), t_values.z);

    inter_point = position_default + max_t * first_direction;

    if (max_t == t_values.x)
    {
        inter_point.x += x * 0.06;
        if (x == -1) orientation = 'X';
        else orientation = 'x';
    }
    else if (max_t == t_values.y)
    {
        inter_point.y += y * 0.06;
        if (y == -1) orientation = 'Y';
        else orientation = 'y';
    }
    else if (max_t == t_values.z)
    {
        inter_point.z += z * 0.06;
        if (z == -1) orientation = 'Z';
        else orientation = 'z';
    }

    return inter_point;
}

bool OctreeManagement::process_octree_children(float t0x, float t0y, float t0z, float t1x, float t1y, float t1z, cube *c, glm::vec3 &inter_point, unsigned char a) {
    bool is_solid;

    unsigned short current_cube;

    if ( (t1x <= 0.0f ) || (t1y <= 0.0f) || (t1z <= 0.0f) )
        return false;


    if (!c->children)
    {
        if (c->solid)
        {
           inter_point = glm::vec3(t0x, t0y, t0z);
           return true;
        }

        return false;
    }

    float tMx = 0.5 * (t0x + t1x);
    float tMy = 0.5 * (t0y + t1y);
    float tMz = 0.5 * (t0z + t1z);

    current_cube = find_first_cube(t0x, t0y, t0z, tMx, tMy, tMz);

    do
    {
        switch (current_cube)
        {
            case 0:
                is_solid = process_octree_children(tMx, tMy, tMz, t1x, t1y, t1z, &c->children[a^6], inter_point, a);
                //cerr<<"case 0\t"<<endl;
                //cerr<<"cube\t"<<int(a^6)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = 8;

                break;

            case 1:
                is_solid = process_octree_children(t0x, tMy, tMz, tMx, t1y, t1z, &c->children[a^7], inter_point, a);
                //cerr<<"case 1\t"<<endl;
                //cerr<<"cube\t"<<int(a^7)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(tMx, t1y, t1z, 0, 8, 8);

                break;

            case 2:
                is_solid = process_octree_children(t0x, tMy, t0z, tMx, t1y, tMz, &c->children[a^4], inter_point, a);
                //cerr<<"case 2\t"<<endl;
                //cerr<<"cube\t"<<int(a^4)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(tMx, t1y, tMz, 3, 8, 1);

                break;

            case 3:
                is_solid = process_octree_children(tMx, tMy, t0z, t1x, t1y, tMz, &c->children[a^5], inter_point, a);
                //cerr<<"case 3\t"<<endl;
                //cerr<<"cube\t"<<int(a^5)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(t1x, t1y, tMz, 8, 8, 0);

                break;

            case 4:
                is_solid = process_octree_children(tMx, t0y, tMz, t1x, tMy, t1z, &c->children[a^2], inter_point, a);
                //cerr<<"case 4\t"<<endl;
                //cerr<<"cube\t"<<int(a^2)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(t1x, tMy, t1z, 8, 0, 8);

                break;

            case 5:
                is_solid = process_octree_children(t0x, t0y, tMz, tMx, tMy, t1z, &c->children[a^3], inter_point, a);
                //cerr<<"case 5\t"<<endl;
                //cerr<<"cube\t"<<int(a^3)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(tMx, tMy, t1z, 4, 1, 8);

                break;

            case 6:
                is_solid = process_octree_children(t0x, t0y, t0z, tMx, tMy, tMz, &c->children[a], inter_point, a);
                //cerr<<"case 6\t"<<endl;
                //cerr<<"cube\t"<<int(a)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(tMx, tMy, tMz, 7, 2, 5);

                break;

            case 7:
                is_solid = process_octree_children(tMx, t0y, t0z, t1x, tMy, tMz, &c->children[a^1], inter_point, a);
                //cerr<<"case 7\t"<<endl;
                //cerr<<"cube\t"<<int(a^1)<<endl<<endl;
                if (is_solid) return true;
                //go to the next cube intersected by the ray
                current_cube = find_next_cube(t1x, tMy, tMz, 8, 3, 4);

                break;
        }
    } while (current_cube < 8);
    return false;
}

unsigned short OctreeManagement::find_first_cube(float t0x, float t0y, float t0z, float tMx, float tMy, float tMz) {
    unsigned char cube_child = 6;
    if (max(max(t0x, t0y), t0z) == t0x)
    {
        //plane YZ
        if (tMy < t0x) cube_child ^= 4;//child 2
        if (tMz < t0x) cube_child ^= 3;//chlid 5
        //if both conditions are true than child will become 1
    }

    if (max(max(t0x, t0y), t0z) == t0y)
    {
        //plane XZ
        if (tMx < t0y) cube_child ^= 1;//child 7
        if (tMz < t0y) cube_child ^= 3;//child 5
        //if both conditions are true then child will become 4
    }

    if (max(max(t0x, t0y), t0z) == t0z)
    {
        //plane XY
        if (tMx < t0z) cube_child ^= 1;//child 7
        if (tMy < t0z) cube_child ^= 4;//child 2
        //if both conditions are true then child will become 3
    }

    return (unsigned short) cube_child;
}

unsigned short OctreeManagement::find_next_cube(float x, float y, float z, unsigned short a, unsigned short b, unsigned short c) {
    //plane YZ
    if (min(min(x, y), z) == x) return a;
    //plane XZ
    if (min(min(x, y), z) == y) return b;
    //plane XY
    if (min(min(x, y), z) == z) return c;
}

/*
glm::vec3 OctreeManagement::process_octree_children2(glm::vec3 t0, glm::vec3 t1, glm::vec3 center, cube *c) {

    glm::vec3 inter_point;

    //unsigned short current_cube;
    //glm::vec3 new_center;
    //glm::vec3 tM = 0.5 * (t0 + t1);
    for(;;)
    {
        if (c->children)
        {
            //find first cube intersected
            glm::vec3 tM = 0.5 * (t0 + t1);

            unsigned short current_cube = find_child_cube(t0, center);
            glm::vec3 new_center = findOctreeCenter(t0, center);
            inter_point = process_octree_children2(t0, tM, new_center, &c->children[current_cube]);
        }
        else
        {
            //verify if the current cube is solid
            if (c->solid) return t0;

            //find next cube intersected
            current_cube = find_child_cube(tM, center);
            //new_center = findOctreeCenter(tM, center);
            inter_point = process_octree_children2(tM, t1, center, &c->children[current_cube]);
        }
        if (inter_point) return inter_point;
    }


   /*
    bool condition = false;

    for (unsigned short i = 0; i < 8; i++)
    {
        float new_radius;
        glm::vec3 new_center;
        cube *c = &octree_data[i];

        new_center = calculate_octree_center(center, radius, i);
        new_radius = radius/2;

        if (c->children)
        {
            condition = is_octree_parent_solid(c->children, new_center, new_radius);
        }
        else
        {
            if ((c->solid)) return true;
        }

        if (condition) return true;
    }
    return condition;

}
*/

void OctreeManagement::findMousePointer(glm::vec3 position, glm::vec3 direction) {
    PointerPosition = position + (2.0f * glm::normalize(direction));
}

void OctreeManagement::update_level(short mouse_scroll) {
    selected = false;

    if(level <= 12 && mouse_scroll == -1)
    {
        level++;
    }
    else if(level > 1 && mouse_scroll == 1)
    {
        level--;
    }
}

glm::vec3 OctreeManagement::findOctreeCenter(glm::vec3 pointer_position, glm::vec3 center, float radius) {
 if(pointer_position.x >= center.x)
 {
  if(pointer_position.y >= center.y)
  {
   if(pointer_position.z >= center.z)
   {
    //cube 0
    glm::vec3 new_center;
    new_center.x = center.x + radius/2;
    new_center.y = center.y + radius/2;
    new_center.z = center.z + radius/2;
    return new_center;
   }
   else if (pointer_position.z < center.z)
   {
    //cube 3
    glm::vec3 new_center;
    new_center.x = center.x + radius/2;
    new_center.y = center.y + radius/2;
    new_center.z = center.z - radius/2;
    return new_center;
   }
  }
  else if (pointer_position.y < center.y)
  {
   if(pointer_position.z >= center.z)
   {
    //cube 4
    glm::vec3 new_center;
    new_center.x = center.x + radius/2;
    new_center.y = center.y - radius/2;
    new_center.z = center.z + radius/2;
    return new_center;
   }
   else if (pointer_position.z < center.z)
   {
    //cube 7
    glm::vec3 new_center;
    new_center.x = center.x + radius/2;
    new_center.y = center.y - radius/2;
    new_center.z = center.z - radius/2;
    return new_center;
   }
  }
 }
 else if (pointer_position.x < center.x)
 {
  if(pointer_position.y >= center.y)
  {
   if(pointer_position.z >= center.z)
   {
    //cube 1
    glm::vec3 new_center;
    new_center.x = center.x - radius/2;
    new_center.y = center.y + radius/2;
    new_center.z = center.z + radius/2;
    return new_center;
   }
   else if (pointer_position.z < center.z)
   {
    //cube 2
    glm::vec3 new_center;
    new_center.x = center.x - radius/2;
    new_center.y = center.y + radius/2;
    new_center.z = center.z - radius/2;
    return new_center;
   }
  }
  else if (pointer_position.y < center.y)
  {
   if(pointer_position.z >= center.z)
   {
    //cube 5
    glm::vec3 new_center;
    new_center.x = center.x - radius/2;
    new_center.y = center.y - radius/2;
    new_center.z = center.z + radius/2;
    return new_center;
   }
   else if (pointer_position.z < center.z)
   {
    //cube 6
    glm::vec3 new_center;
    new_center.x = center.x - radius/2;
    new_center.y = center.y - radius/2;
    new_center.z = center.z - radius/2;
    return new_center;
   }
  }
 }
}

void OctreeManagement::updateCubeFace(glm::vec3 position, glm::vec3 direction, GLushort window_width, GLushort window_height, MVPmatrices matrix) {
    //findMousePointer(position, direction);
    glm::vec3 inter_point;
    inter_point = ray_octree_intersection(position, glm::normalize(direction));
    //inter_point = return_intersection_point(position, direction);
    //inter_point = return_intersection_point2(position, direction, window_width, window_height, matrix);
    //cerr<<"X\t"<<inter_point.x<<endl;
    //cerr<<"Y\t"<<inter_point.y<<endl;
    //cerr<<"Z\t"<<inter_point.z<<endl;

    //cerr<<"inter_point.x\t"<<inter_point.x<<endl;
    //cerr<<"inter_point.y\t"<<inter_point.y<<endl;
    //cerr<<"inter_point.z\t"<<inter_point.z<<endl;

    float new_radius = world_radius;
    glm::vec3 new_center = world_center;
    data_selection cube_data;

    for(unsigned short i = 0; i < level; i++)
    {
        new_center = findOctreeCenter(inter_point, new_center, new_radius);
        new_radius = new_radius/2;
    }

    cube_data = generate_cube_vertices(new_center, new_radius, false);
    store_data_to_gpu(cube_data);
    if (on_click)
    {
        cube_selection = cube_data;
        selection_orientation = orientation;
        on_click = false;
    }
}

void OctreeManagement::generate_buffer_objects() {
 glGenVertexArrays(1, &vaoID);
 glGenBuffers(1, &vboVerticesID);
 glGenBuffers(1, &iboVerticesID);
}

void OctreeManagement::store_data_to_gpu(data_selection cube_data) {
 glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data.vertices), cube_data.vertices, GL_STATIC_DRAW);
  ExitOnGLError("ERROR: Could not create --oct_vertices-- BufferData");
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  ExitOnGLError("ERROR: glVertexAttribArray() --vsVertex-- FAILED");
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVerticesID);
  ExitOnGLError("ERROR: glBindBuffer() --iboVertex-- FAILED");
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_data.elements), cube_data.elements, GL_STATIC_DRAW);
  ExitOnGLError("ERROR: glBufferData() --iboVertex-- FAILED");

 glBindVertexArray(0);
}

data_selection OctreeManagement::generate_cube_vertices(glm::vec3 center, float radius, bool cube_selection) {

 data_selection cube_data;

 cube_data.vertices[4] = glm::vec3(center.x - radius, center.y - radius, center.z + radius);
 cube_data.vertices[2] = glm::vec3(center.x + radius, center.y + radius, center.z - radius);

 cube_data.vertices[0] = glm::vec3(cube_data.vertices[4].x, cube_data.vertices[2].y, cube_data.vertices[4].z);
 cube_data.vertices[1] = glm::vec3(cube_data.vertices[4].x, cube_data.vertices[2].y, cube_data.vertices[2].z);
 cube_data.vertices[3] = glm::vec3(cube_data.vertices[2].x, cube_data.vertices[2].y, cube_data.vertices[4].z);
 cube_data.vertices[5] = glm::vec3(cube_data.vertices[4].x, cube_data.vertices[4].y, cube_data.vertices[2].z);
 cube_data.vertices[6] = glm::vec3(cube_data.vertices[2].x, cube_data.vertices[4].y, cube_data.vertices[2].z);
 cube_data.vertices[7] = glm::vec3(cube_data.vertices[2].x, cube_data.vertices[4].y, cube_data.vertices[4].z);

 //verify which cube face is selected
 GLubyte front_face[4];
 GLubyte back_face[4];
 char orient;

    if (cube_selection)
    {
        orient = selection_orientation;
    }
    else
    {
        orient = orientation;
    }

 if(orient == 'X')
 {
  front_face[0] = 3;
  front_face[1] = 2;
  front_face[2] = 6;
  front_face[3] = 7;

  back_face[0] = 0;
  back_face[1] = 1;
  back_face[2] = 5;
  back_face[3] = 4;
 }
 else if(orient == 'x')
 {
  front_face[0] = 0;
  front_face[1] = 1;
  front_face[2] = 5;
  front_face[3] = 4;

  back_face[0] = 3;
  back_face[1] = 2;
  back_face[2] = 6;
  back_face[3] = 7;
 }
 else if(orient == 'Y')
 {
  front_face[0] = 1;
  front_face[1] = 2;
  front_face[2] = 3;
  front_face[3] = 0;

  back_face[0] = 5;
  back_face[1] = 6;
  back_face[2] = 7;
  back_face[3] = 4;
 }
 else if(orient == 'y')
 {
  front_face[0] = 5;
  front_face[1] = 6;
  front_face[2] = 7;
  front_face[3] = 4;

  back_face[0] = 1;
  back_face[1] = 2;
  back_face[2] = 3;
  back_face[3] = 0;
 }
 else if(orient == 'Z')
 {
  front_face[0] = 0;
  front_face[1] = 3;
  front_face[2] = 7;
  front_face[3] = 4;

  back_face[0] = 1;
  back_face[1] = 2;
  back_face[2] = 6;
  back_face[3] = 5;
 }
 else if(orient == 'z')
 {
  front_face[0] = 1;
  front_face[1] = 2;
  front_face[2] = 6;
  front_face[3] = 5;

  back_face[0] = 0;
  back_face[1] = 3;
  back_face[2] = 7;
  back_face[3] = 4;
 }
 //end

 //create ibo for the gpu
 cube_data.elements[0]  = front_face[0];
 cube_data.elements[1]  = front_face[1];
 cube_data.elements[2]  = front_face[2];
 cube_data.elements[3]  = front_face[3];

 cube_data.elements[4]  = back_face[0];
 cube_data.elements[5]  = back_face[1];
 cube_data.elements[6]  = back_face[2];
 cube_data.elements[7]  = back_face[3];

 cube_data.elements[8]  = front_face[0];
 cube_data.elements[9]  = back_face[0];
 cube_data.elements[10] = front_face[1];
 cube_data.elements[11] = back_face[1];
 cube_data.elements[12] = front_face[2];
 cube_data.elements[13] = back_face[2];
 cube_data.elements[14] = front_face[3];
 cube_data.elements[15] = back_face[3];
 //end

 cube_data.center = center;
 cube_data.radius = radius;

 return cube_data;
}

void OctreeManagement::keyG_pressed(bool condition) {
    keyG = condition;
}

bool OctreeManagement::return_keyG(void) {
    return keyG;
}

void OctreeManagement::mark_selection() {
    on_click = true;
    selected = true;
    //cerr<<"clicked the mouse !!!"<<endl;
}

void OctreeManagement::unmark_selection() {
    selected = false;
}

bool OctreeManagement::if_selected() {
    return selected;
}

void OctreeManagement::setup_octree(GLuint shader) {
    uniform_color = glGetUniformLocation(shader, "color");
    generate_octree();
    generate_buffer_objects();
    ExitOnGLError("ERROR: generate_buffer_objects ! ");
}

void OctreeManagement::Draw(GLuint shader, MVPuniforms uniform, MVPmatrices matrix) {
    render_selection_cubes(shader, uniform, matrix);
    if(selected)
    {
        store_data_to_gpu(cube_selection);
        render_selection_cubes(shader, uniform, matrix);
    }
}

void OctreeManagement::render_selection_cubes(GLuint shader, MVPuniforms uniform, MVPmatrices matrix) {

 glUseProgram(shader);
 ExitOnGLError("ERROR: glUseProgram ");
 glUniformMatrix4fv(uniform.Proj, 1, GL_FALSE, glm::value_ptr(matrix.Projection));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.Proj) ");
 glUniformMatrix4fv(uniform.View, 1, GL_FALSE, glm::value_ptr(matrix.View));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.View) ");
 glUniformMatrix4fv(uniform.Model, 1, GL_FALSE, glm::value_ptr(matrix.Model));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.Model) ");


 glBindVertexArray(vaoID);
 ExitOnGLError("ERROR: vaoID ");
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVerticesID);
  ExitOnGLError("ERROR: iboVerticesID ");

  GLfloat color[4] = {1.0, 1.0, 1.0, 1.0};
  glUniform4fv(uniform_color, 1, color);
  glLineWidth(2);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, 0);
  ExitOnGLError("ERROR: draw1 ");

  GLfloat color2[4] = {0.0, 0.0, 1.0, 1.0};
  glUniform4fv(uniform_color, 1, color2);
  glLineWidth(1);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (GLvoid*)(4*sizeof(GLubyte)));
  ExitOnGLError("ERROR: draw2 ");
  glDrawElements(GL_LINES, 8, GL_UNSIGNED_BYTE, (GLvoid*)(8*sizeof(GLubyte)));
  ExitOnGLError("ERROR: draw3 ");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

 glBindVertexArray(0);

 glUseProgram(0);
}

//render map functions////////////////////////////////
//////////////////////////////////////////////////////

void OctreeManagement::MapGeneration(cube *map_data) {
 world_center = glm::vec3(512.0f, 512.0f, 512.0f);
 world_radius = 512.0f;
 //world_center = glm::vec3(204.8f, 204.8f, 204.8f);
 //world_radius = 204.8;

 vector<vector_data> map_data_vertices;

 traverse_octree_data(map_data, world_center, world_radius, map_data_vertices);

 nr_vertices = map_data_vertices.size() * sizeof(map_data_vertices[0]) / sizeof(map_data_vertices[0].face[0].vertices[0]);

 store_data_to_gpu2(map_data_vertices);

}

void OctreeManagement::traverse_octree_data(cube *octree_data, glm::vec3 center, float radius, vector<vector_data> &map_data_vertices) {
    for (unsigned short i = 0; i < 8; i++)
    {
        float new_radius;
        glm::vec3 new_center;
        cube *c = &octree_data->children[i];

        new_center = calculate_octree_center(center, radius, i);
        new_radius = radius/2;
        //cerr<<"new_radius\t"<<new_radius<<endl;

        if (c->children)
        {
            traverse_octree_data(c, new_center, new_radius, map_data_vertices);
        }
        else
        {
            //if cube is not solid move to the next one
            if (!(c->solid)) continue;
            //get the 12 egde_spans and generate vertices based on them
            create_vector_map(generate_cube_final_vertices(c, new_center, new_radius), map_data_vertices, c);
            //cerr<<"octree_cube\t"<<endl<<endl;
        }
    }
}

cube_vertices OctreeManagement::generate_cube_final_vertices(cube *c, glm::vec3 center, float radius) {

 cube_vertices Vert = calculate_cube_vertices(center, radius);
 float_edge_span edges[12];

  //face x
  edges[0].start.x  = Vert.vertices[1].x + (int(c->edges[0].start) * radius/4);
  edges[0].start.y  = Vert.vertices[1].y;
  edges[0].start.z  = Vert.vertices[1].z;
  edges[0].ending.x = Vert.vertices[0].x - ((8 - int(c->edges[0].ending)) * radius/4);
  edges[0].ending.y = Vert.vertices[0].y;
  edges[0].ending.z = Vert.vertices[0].z;

  edges[1].start.x  = Vert.vertices[2].x + (int(c->edges[1].start) * radius/4);
  edges[1].start.y  = Vert.vertices[2].y;
  edges[1].start.z  = Vert.vertices[2].z;
  edges[1].ending.x = Vert.vertices[3].x - ((8 - int(c->edges[1].ending)) * radius/4);
  edges[1].ending.y = Vert.vertices[3].y;
  edges[1].ending.z = Vert.vertices[3].z;

  edges[2].start.x  = Vert.vertices[5].x + (int(c->edges[2].start) * radius/4);
  edges[2].start.y  = Vert.vertices[5].y;
  edges[2].start.z  = Vert.vertices[5].z;
  edges[2].ending.x = Vert.vertices[4].x - ((8 - int(c->edges[2].ending)) * radius/4);
  edges[2].ending.y = Vert.vertices[4].y;
  edges[2].ending.z = Vert.vertices[4].z;

  edges[3].start.x  = Vert.vertices[6].x + (int(c->edges[3].start) * radius/4);
  edges[3].start.y  = Vert.vertices[6].y;
  edges[3].start.z  = Vert.vertices[6].z;
  edges[3].ending.x = Vert.vertices[7].x - ((8 - int(c->edges[3].ending)) * radius/4);
  edges[3].ending.y = Vert.vertices[7].y;
  edges[3].ending.z = Vert.vertices[7].z;

  //face y
  edges[4].start.x  = Vert.vertices[7].x;
  edges[4].start.y  = Vert.vertices[7].y + (int(c->edges[4].start) * radius/4);
  edges[4].start.z  = Vert.vertices[7].z;
  edges[4].ending.x = Vert.vertices[3].x;
  edges[4].ending.y = Vert.vertices[3].y - ((8 - int(c->edges[4].ending)) * radius/4);
  edges[4].ending.z = Vert.vertices[3].z;

  edges[5].start.x  = Vert.vertices[4].x;
  edges[5].start.y  = Vert.vertices[4].y + (int(c->edges[5].start) * radius/4);
  edges[5].start.z  = Vert.vertices[4].z;
  edges[5].ending.x = Vert.vertices[0].x;
  edges[5].ending.y = Vert.vertices[0].y - ((8 - int(c->edges[5].ending)) * radius/4);
  edges[5].ending.z = Vert.vertices[0].z;

  edges[6].start.x  = Vert.vertices[6].x;
  edges[6].start.y  = Vert.vertices[6].y + (int(c->edges[6].start) * radius/4);
  edges[6].start.z  = Vert.vertices[6].z;
  edges[6].ending.x = Vert.vertices[2].x;
  edges[6].ending.y = Vert.vertices[2].y - ((8 - int(c->edges[6].ending)) * radius/4);
  edges[6].ending.z = Vert.vertices[2].z;

  edges[7].start.x  = Vert.vertices[5].x;
  edges[7].start.y  = Vert.vertices[5].y + (int(c->edges[7].start) * radius/4);
  edges[7].start.z  = Vert.vertices[5].z;
  edges[7].ending.x = Vert.vertices[1].x;
  edges[7].ending.y = Vert.vertices[1].y - ((8 - int(c->edges[7].ending)) * radius/4);
  edges[7].ending.z = Vert.vertices[1].z;

  //face z
  edges[8].start.x  = Vert.vertices[2].x;
  edges[8].start.y  = Vert.vertices[2].y;
  edges[8].start.z  = Vert.vertices[2].z + (int(c->edges[8].start) * radius/4);
  edges[8].ending.x = Vert.vertices[1].x;
  edges[8].ending.y = Vert.vertices[1].y;
  edges[8].ending.z = Vert.vertices[1].z - ((8 - int(c->edges[8].ending)) * radius/4);

  edges[9].start.x  = Vert.vertices[3].x;
  edges[9].start.y  = Vert.vertices[3].y;
  edges[9].start.z  = Vert.vertices[3].z + (int(c->edges[9].start) * radius/4);
  edges[9].ending.x = Vert.vertices[0].x;
  edges[9].ending.y = Vert.vertices[0].y;
  edges[9].ending.z = Vert.vertices[0].z - ((8 - int(c->edges[9].ending)) * radius/4);

  edges[10].start.x  = Vert.vertices[6].x;
  edges[10].start.y  = Vert.vertices[6].y;
  edges[10].start.z  = Vert.vertices[6].z + (int(c->edges[10].start) * radius/4);
  edges[10].ending.x = Vert.vertices[5].x;
  edges[10].ending.y = Vert.vertices[5].y;
  edges[10].ending.z = Vert.vertices[5].z - ((8 - int(c->edges[10].ending)) * radius/4);

  edges[11].start.x  = Vert.vertices[7].x;
  edges[11].start.y  = Vert.vertices[7].y;
  edges[11].start.z  = Vert.vertices[7].z + (int(c->edges[11].start) * radius/4);
  edges[11].ending.x = Vert.vertices[4].x;
  edges[11].ending.y = Vert.vertices[4].y;
  edges[11].ending.z = Vert.vertices[4].z - ((8 - int(c->edges[11].ending)) * radius/4);

  //calculate final vertices
  Vert.vertices[0].x = calculate_axis('x', edges[0].start.x, edges[0].start.y, edges[5].ending.x, edges[5].ending.y, edges[5].start.x, edges[5].start.y, edges[0].ending.x, edges[0].ending.y);
  Vert.vertices[0].y = calculate_axis('y', edges[0].start.x, edges[0].start.y, edges[5].ending.x, edges[5].ending.y, edges[5].start.x, edges[5].start.y, edges[0].ending.x, edges[0].ending.y);
  Vert.vertices[0].z = calculate_axis('z', edges[0].start.x, edges[0].start.z, edges[9].ending.x, edges[9].ending.z, edges[9].start.x, edges[9].start.z, edges[0].ending.x, edges[0].ending.z);

  Vert.vertices[1].x = calculate_axis('x', edges[7].start.x, edges[7].start.y, edges[0].start.x, edges[0].start.y, edges[0].ending.x, edges[0].ending.y, edges[7].ending.x, edges[7].ending.y);
  Vert.vertices[1].y = calculate_axis('y', edges[7].start.x, edges[7].start.y, edges[0].start.x, edges[0].start.y, edges[0].ending.x, edges[0].ending.y, edges[7].ending.x, edges[7].ending.y);
  Vert.vertices[1].z = calculate_axis('z', edges[8].start.x, edges[8].start.z, edges[0].start.x, edges[0].start.z, edges[0].ending.x, edges[0].ending.z, edges[8].ending.x, edges[8].ending.z);

  Vert.vertices[2].x = calculate_axis('x', edges[6].start.x, edges[6].start.y, edges[1].start.x, edges[1].start.y, edges[1].ending.x, edges[1].ending.y, edges[6].ending.x, edges[6].ending.y);
  Vert.vertices[2].y = calculate_axis('y', edges[6].start.x, edges[6].start.y, edges[1].start.x, edges[1].start.y, edges[1].ending.x, edges[1].ending.y, edges[6].ending.x, edges[6].ending.y);
  Vert.vertices[2].z = calculate_axis('z', edges[8].ending.x, edges[8].ending.z, edges[1].start.x, edges[1].start.z, edges[1].ending.x, edges[1].ending.z, edges[8].start.x, edges[8].start.z);

  Vert.vertices[3].x = calculate_axis('x', edges[1].start.x, edges[1].start.y, edges[4].ending.x, edges[4].ending.y, edges[4].start.x, edges[4].start.y, edges[1].ending.x, edges[1].ending.y);
  Vert.vertices[3].y = calculate_axis('y', edges[1].start.x, edges[1].start.y, edges[4].ending.x, edges[4].ending.y, edges[4].start.x, edges[4].start.y, edges[1].ending.x, edges[1].ending.y);
  Vert.vertices[3].z = calculate_axis('z', edges[1].start.x, edges[1].start.z, edges[9].start.x, edges[9].start.z, edges[9].ending.x, edges[9].ending.z, edges[1].ending.x, edges[1].ending.z);

  Vert.vertices[4].x = calculate_axis('x', edges[2].start.x, edges[2].start.y, edges[5].start.x, edges[5].start.y, edges[5].ending.x, edges[5].ending.y, edges[2].ending.x, edges[2].ending.y);
  Vert.vertices[4].y = calculate_axis('y', edges[2].start.x, edges[2].start.y, edges[5].start.x, edges[5].start.y, edges[5].ending.x, edges[5].ending.y, edges[2].ending.x, edges[2].ending.y);
  Vert.vertices[4].z = calculate_axis('z', edges[2].start.x, edges[2].start.z, edges[11].ending.x, edges[11].ending.z, edges[11].start.x, edges[11].start.z, edges[2].ending.x, edges[2].ending.z);

  Vert.vertices[5].x = calculate_axis('x', edges[7].ending.x, edges[7].ending.y, edges[2].start.x, edges[2].start.y, edges[2].ending.x, edges[2].ending.y, edges[7].start.x, edges[7].start.y);
  Vert.vertices[5].y = calculate_axis('y', edges[7].ending.x, edges[7].ending.y, edges[2].start.x, edges[2].start.y, edges[2].ending.x, edges[2].ending.y, edges[7].start.x, edges[7].start.y);
  Vert.vertices[5].z = calculate_axis('z', edges[10].start.x, edges[10].start.z, edges[2].start.x, edges[2].start.z, edges[2].ending.x, edges[2].ending.z, edges[10].ending.x, edges[10].ending.z);

  Vert.vertices[6].x = calculate_axis('x', edges[6].ending.x, edges[6].ending.y, edges[3].start.x, edges[3].start.y, edges[3].ending.x, edges[3].ending.y, edges[6].start.x, edges[6].start.y);
  Vert.vertices[6].y = calculate_axis('y', edges[6].ending.x, edges[6].ending.y, edges[3].start.x, edges[3].start.y, edges[3].ending.x, edges[3].ending.y, edges[6].start.x, edges[6].start.y);
  Vert.vertices[6].z = calculate_axis('z', edges[10].ending.x, edges[10].ending.z, edges[3].start.x, edges[3].start.z, edges[3].ending.x, edges[3].ending.z, edges[10].start.x, edges[10].start.z);

  Vert.vertices[7].x = calculate_axis('x', edges[3].start.x, edges[3].start.y, edges[4].start.x, edges[4].start.y, edges[4].ending.x, edges[4].ending.y, edges[3].ending.x, edges[3].ending.y);
  Vert.vertices[7].y = calculate_axis('y', edges[3].start.x, edges[3].start.y, edges[4].start.x, edges[4].start.y, edges[4].ending.x, edges[4].ending.y, edges[3].ending.x, edges[3].ending.y);
  Vert.vertices[7].z = calculate_axis('z', edges[3].start.x, edges[3].start.z, edges[11].start.x, edges[11].start.z, edges[11].ending.x, edges[11].ending.z, edges[3].ending.x, edges[3].ending.z);

  return Vert;
}

void OctreeManagement::generate_cube_face(vector_data &CubeFaces, cube_vertices Vert, cube *c, unsigned short face_index, unsigned short luc_vertex, unsigned short lbc_vertex, unsigned short ruc_vertex, unsigned short rbc_vertex) {
    if (c->face[face_index].lbc && c->face[face_index].luc ||
        c->face[face_index].lbc && c->face[face_index].rbc ||
        c->face[face_index].lbc && c->face[face_index].ruc ||
        c->face[face_index].luc && c->face[face_index].rbc ||
        c->face[face_index].luc && c->face[face_index].ruc ||
        c->face[face_index].rbc && c->face[face_index].ruc)
    {
        CubeFaces.face[face_index].vertices[0] = Vert.vertices[luc_vertex];
        CubeFaces.face[face_index].vertices[1] = Vert.vertices[lbc_vertex];
        CubeFaces.face[face_index].vertices[2] = Vert.vertices[rbc_vertex];
        CubeFaces.face[face_index].vertices[3] = Vert.vertices[rbc_vertex];
        CubeFaces.face[face_index].vertices[4] = Vert.vertices[ruc_vertex];
        CubeFaces.face[face_index].vertices[5] = Vert.vertices[luc_vertex];
    }
    else if (c->face[face_index].lbc)
    {
        //make some magic to separate all 6 triangles; we must draw them separately
        //first triangle
        CubeFaces.face[face_index].vertices[0] = Vert.vertices[luc_vertex];
        CubeFaces.face[face_index].vertices[1] = Vert.vertices[lbc_vertex];
        CubeFaces.face[face_index].vertices[2] = Vert.vertices[rbc_vertex];
    }
    else if (c->face[face_index].ruc)
    {
        //second triangle
        CubeFaces.face[face_index].vertices[3] = Vert.vertices[rbc_vertex];
        CubeFaces.face[face_index].vertices[4] = Vert.vertices[ruc_vertex];
        CubeFaces.face[face_index].vertices[5] = Vert.vertices[luc_vertex];
    }
    else if (c->face[face_index].rbc)
    {
        //third triangle
        CubeFaces.face[face_index].vertices[0] = Vert.vertices[lbc_vertex];
        CubeFaces.face[face_index].vertices[1] = Vert.vertices[rbc_vertex];
        CubeFaces.face[face_index].vertices[2] = Vert.vertices[ruc_vertex];
    }
    else if(c->face[face_index].luc)
    {
        //fourth triangle
        CubeFaces.face[face_index].vertices[3] = Vert.vertices[ruc_vertex];
        CubeFaces.face[face_index].vertices[4] = Vert.vertices[luc_vertex];
        CubeFaces.face[face_index].vertices[5] = Vert.vertices[lbc_vertex];
    }
}

void OctreeManagement::create_vector_map(cube_vertices Vert, vector<vector_data> &map_data_vertices, cube *c) {

    vector_data CubeFaces;
/*
    //face x
    if (c->face[0])
    {
        CubeFaces.face[0].vertices[0] = Vert.vertices[0];
        CubeFaces.face[0].vertices[1] = Vert.vertices[4];
        CubeFaces.face[0].vertices[2] = Vert.vertices[7];
        CubeFaces.face[0].vertices[3] = Vert.vertices[7];
        CubeFaces.face[0].vertices[4] = Vert.vertices[3];
        CubeFaces.face[0].vertices[5] = Vert.vertices[0];
    }


    //face -x
    if (c->face[1])
    {
        CubeFaces.face[1].vertices[0] = Vert.vertices[2];
        CubeFaces.face[1].vertices[1] = Vert.vertices[6];
        CubeFaces.face[1].vertices[2] = Vert.vertices[5];
        CubeFaces.face[1].vertices[3] = Vert.vertices[5];
        CubeFaces.face[1].vertices[4] = Vert.vertices[1];
        CubeFaces.face[1].vertices[5] = Vert.vertices[2];
    }


    //face y
    if (c->face[2])
    {
        CubeFaces.face[2].vertices[0] = Vert.vertices[2];
        CubeFaces.face[2].vertices[1] = Vert.vertices[1];
        CubeFaces.face[2].vertices[2] = Vert.vertices[0];
        CubeFaces.face[2].vertices[3] = Vert.vertices[0];
        CubeFaces.face[2].vertices[4] = Vert.vertices[3];
        CubeFaces.face[2].vertices[5] = Vert.vertices[2];
    }


    //face -y
    if (c->face[3])
    {
        CubeFaces.face[3].vertices[0] = Vert.vertices[5];
        CubeFaces.face[3].vertices[1] = Vert.vertices[6];
        CubeFaces.face[3].vertices[2] = Vert.vertices[7];
        CubeFaces.face[3].vertices[3] = Vert.vertices[7];
        CubeFaces.face[3].vertices[4] = Vert.vertices[4];
        CubeFaces.face[3].vertices[5] = Vert.vertices[5];
    }


    //face z
    if (c->face[4])
    {
        CubeFaces.face[4].vertices[0] = Vert.vertices[1];
        CubeFaces.face[4].vertices[1] = Vert.vertices[5];
        CubeFaces.face[4].vertices[2] = Vert.vertices[4];
        CubeFaces.face[4].vertices[3] = Vert.vertices[4];
        CubeFaces.face[4].vertices[4] = Vert.vertices[0];
        CubeFaces.face[4].vertices[5] = Vert.vertices[1];
    }


    //face -z
    if (c->face[5].lbc && c->face[5].luc ||
        c->face[5].lbc && c->face[5].rbc ||
        c->face[5].lbc && c->face[5].ruc ||
        c->face[5].luc && c->face[5].rbc ||
        c->face[5].luc && c->face[5].ruc ||
        c->face[5].rbc && c->face[5].ruc )
    {
        CubeFaces.face[5].vertices[0] = Vert.vertices[3];
        CubeFaces.face[5].vertices[1] = Vert.vertices[7];
        CubeFaces.face[5].vertices[2] = Vert.vertices[6];
        CubeFaces.face[5].vertices[3] = Vert.vertices[6];
        CubeFaces.face[5].vertices[4] = Vert.vertices[2];
        CubeFaces.face[5].vertices[5] = Vert.vertices[3];
    }
    else if (c->face[5].lbc)
    {
        //make some magic to separate all 6 triangles; we must draw them separately
        //first triangle
        CubeFaces.face[5].vertices[0] = Vert.vertices[3];
        CubeFaces.face[5].vertices[1] = Vert.vertices[7];
        CubeFaces.face[5].vertices[2] = Vert.vertices[6];
    }
    else if (c->face[5].ruc)
    {
        //second triangle
        CubeFaces.face[5].vertices[3] = Vert.vertices[6];
        CubeFaces.face[5].vertices[4] = Vert.vertices[2];
        CubeFaces.face[5].vertices[5] = Vert.vertices[3];
    }
    else if (c->face[5].rbc)
    {
        //third triangle
        CubeFaces.face[5].vertices[0] = Vert.vertices[7];
        CubeFaces.face[5].vertices[1] = Vert.vertices[6];
        CubeFaces.face[5].vertices[2] = Vert.vertices[2];
    }
    else if(c->face[5].luc)
    {
        //fourth triangle
        CubeFaces.face[5].vertices[3] = Vert.vertices[2];
        CubeFaces.face[5].vertices[4] = Vert.vertices[3];
        CubeFaces.face[5].vertices[5] = Vert.vertices[7];
    } */

    //face x
    generate_cube_face(CubeFaces, Vert, c, 0, 0, 4, 3, 7);
    //face -x
    generate_cube_face(CubeFaces, Vert, c, 1, 2, 6, 1, 5);
    //face y
    generate_cube_face(CubeFaces, Vert, c, 2, 2, 1, 3, 0);
    //face -y
    generate_cube_face(CubeFaces, Vert, c, 3, 5, 6, 4, 7);
    //face z
    generate_cube_face(CubeFaces, Vert, c, 4, 1, 5, 0, 4);
    //face -z
    generate_cube_face(CubeFaces, Vert, c, 5, 3, 7, 2, 6);


    map_data_vertices.push_back(CubeFaces);
}

float OctreeManagement::calculate_axis(char axis, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    float float_value;
    if (axis == 'x')
    {
        float_value = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4))/((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
    }
    else if (axis == 'y' || axis == 'z')
    {
        float_value = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4))/((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
        //return y;
    }
    return float_value;
}

cube_vertices OctreeManagement::calculate_cube_vertices(glm::vec3 center, float radius) {

 cube_vertices Vert;

 Vert.vertices[6] = glm::vec3(center.x - radius, center.y - radius, center.z - radius);
 Vert.vertices[0] = glm::vec3(center.x + radius, center.y + radius, center.z + radius);

 Vert.vertices[1] = glm::vec3(Vert.vertices[6].x, Vert.vertices[0].y, Vert.vertices[0].z);
 Vert.vertices[2] = glm::vec3(Vert.vertices[6].x, Vert.vertices[0].y, Vert.vertices[6].z);
 Vert.vertices[3] = glm::vec3(Vert.vertices[0].x, Vert.vertices[0].y, Vert.vertices[6].z);
 Vert.vertices[4] = glm::vec3(Vert.vertices[0].x, Vert.vertices[6].y, Vert.vertices[0].z);
 Vert.vertices[5] = glm::vec3(Vert.vertices[6].x, Vert.vertices[6].y, Vert.vertices[0].z);
 Vert.vertices[7] = glm::vec3(Vert.vertices[0].x, Vert.vertices[6].y, Vert.vertices[6].z);

 return Vert;
}

glm::vec3 OctreeManagement::calculate_octree_center(glm::vec3 center, float radius, unsigned short i) {
 glm::vec3 new_center;
 switch (i)
 {
  case 0:
   new_center.x = center.x + radius/2;
   new_center.y = center.y + radius/2;
   new_center.z = center.z + radius/2;
   break;

  case 1:
   new_center.x = center.x - radius/2;
   new_center.y = center.y + radius/2;
   new_center.z = center.z + radius/2;
   break;

  case 2:
   new_center.x = center.x - radius/2;
   new_center.y = center.y + radius/2;
   new_center.z = center.z - radius/2;
   break;

  case 3:
   new_center.x = center.x + radius/2;
   new_center.y = center.y + radius/2;
   new_center.z = center.z - radius/2;
   break;

  case 4:
   new_center.x = center.x + radius/2;
   new_center.y = center.y - radius/2;
   new_center.z = center.z + radius/2;
   break;

  case 5:
   new_center.x = center.x - radius/2;
   new_center.y = center.y - radius/2;
   new_center.z = center.z + radius/2;
   break;

  case 6:
   new_center.x = center.x - radius/2;
   new_center.y = center.y - radius/2;
   new_center.z = center.z - radius/2;
   break;

  case 7:
   new_center.x = center.x + radius/2;
   new_center.y = center.y - radius/2;
   new_center.z = center.z - radius/2;
   break;
 }
  return new_center;
}

void OctreeManagement::generate_buffer_objects2() {
 glGenVertexArrays(1, &vaoID);
 glGenBuffers(1, &vboVerticesID);
 //glGenBuffers(1, &iboVerticesID);
}

void OctreeManagement::store_data_to_gpu2(vector<vector_data> map_data) {
 glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
  glBufferData(GL_ARRAY_BUFFER, map_data.size() * sizeof(map_data[0]), &map_data[0], GL_STATIC_DRAW);
  ExitOnGLError("ERROR: Could not create --map_vertices-- BufferData");
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  ExitOnGLError("ERROR: glVertexAttribArray() --vsVertex-- FAILED");
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboVerticesID);
  //ExitOnGLError("ERROR: glBindBuffer() --iboVertex-- FAILED");
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_data.elements), cube_data.elements, GL_STATIC_DRAW);
  //ExitOnGLError("ERROR: glBufferData() --iboVertex-- FAILED");

 glBindVertexArray(0);
}

void OctreeManagement::Render(GLuint shader, MVPuniforms uniform, MVPmatrices matrix) {

 glUseProgram(shader);
 ExitOnGLError("ERROR: glUseProgram ");
 glUniformMatrix4fv(uniform.Proj, 1, GL_FALSE, glm::value_ptr(matrix.Projection));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.Proj) ");
 glUniformMatrix4fv(uniform.View, 1, GL_FALSE, glm::value_ptr(matrix.View));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.View) ");
 glUniformMatrix4fv(uniform.Model, 1, GL_FALSE, glm::value_ptr(matrix.Model));
 ExitOnGLError("ERROR: glUniformMatrix4fv(uniform.Model) ");


 glBindVertexArray(vaoID);
 ExitOnGLError("ERROR: vaoID ");

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  GLfloat color[4] = {1.0, 0.0, 0.0, 1.0};
  glUniform4fv(uniform_color, 1, color);

  glDrawArrays(GL_TRIANGLES, 0, nr_vertices);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

 glBindVertexArray(0);

 glUseProgram(0);
}

void OctreeManagement::setup_octree2(GLuint shader) {
 uniform_color = glGetUniformLocation(shader, "color");
 generate_buffer_objects2();
 ExitOnGLError("ERROR: generate_buffer_objects ! ");
}
