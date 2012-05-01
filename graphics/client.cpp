/**
 * @file client.cpp
 *
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-07
 */

#include "../maps/maze.hpp"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Texture2D>
#include <osg/PolygonStipple>
#include <osg/TriangleFunctor>
#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/TrackballManipulator>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Math>

#include <iostream>

#include <memory>

#include <boost/multi_array.hpp>

class PhysicsObject {
public:
    PhysicsObject(osg::ref_ptr<osg::PositionAttitudeTransform> xform)
        : velocity(osg::Vec3d(0,0,0))
        , omega(osg::Quat(0, osg::Vec3d(1,0,0)))
        , xform(xform)
    {}

    void add_vel(osg::Vec3d v) {
        velocity += xform->getAttitude() * v;
    }

    void add_rot(osg::Quat q) {
        omega *= q;
    }

    void move() {
        xform->setPosition(xform->getPosition() + velocity);
        xform->setAttitude(xform->getAttitude() * omega);
    }

    void clear() {
        velocity = osg::Vec3d(0,0,0);
        omega    = osg::Quat(0, osg::Vec3d(1,0,0));
    }

    osg::Vec3d velocity;
    osg::Quat  omega;

    osg::ref_ptr<osg::PositionAttitudeTransform> xform;
};

osg::ref_ptr<osg::Geode>
ijk()
{
    osg::ref_ptr<osg::Geode> parent = new osg::Geode();
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    parent->addDrawable(geometry);

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f)); /* middle */
    colors->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f)); /* i */
    colors->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f)); /* j */
    colors->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f)); /* k */


    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();
    points->push_back(osg::Vec3(0.f, 0.f, 0.f)); /* middle */
    points->push_back(osg::Vec3(1.f, 0.f, 0.f)); /* i */
    points->push_back(osg::Vec3(0.f, 1.f, 0.f)); /* j */
    points->push_back(osg::Vec3(0.f, 0.f, 1.f)); /* k */

    osg::ref_ptr<osg::DrawElementsUInt> lines =
        new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
    lines->push_back(0);
    lines->push_back(1);
    lines->push_back(0);
    lines->push_back(2);
    lines->push_back(0);
    lines->push_back(3);

    geometry->setVertexArray(points);
    geometry->setColorArray(colors);
    geometry->addPrimitiveSet(lines);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    parent->getOrCreateStateSet()->setMode( GL_LIGHTING,
            osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );

    return parent;
}

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardEventHandler() : pressed() {}

    std::set<int> pressed;

    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
    {
        //std::cout << ea.getEventType() << "\t";
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                pressed.insert(ea.getKey());

                if (ea.getKey() == 65307) {
                    std::cout << "Escape pressed, exiting" << std::endl;
                    exit(0);
                }
            }break;
            case(osgGA::GUIEventAdapter::KEYUP):
            {
                auto it = pressed.find(ea.getKey());
                if (it != pressed.end()) {
                    pressed.erase(it);
                }
            }break;
            default:
                break;
        }
        return true;
    }

    void apply_commands(PhysicsObject& obj) {
        double movespeed =
            0.027869470349083095035789384576097879050347563756563587097654766897654;
        for (auto& key : pressed) {
            switch(key)
            {
                case(osgGA::GUIEventAdapter::KEY_C):
                {
                    obj.add_vel(osg::Vec3d(0.0,-movespeed,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Space):
                {
                    obj.add_vel(osg::Vec3d(0.0,movespeed,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Left):
                case(osgGA::GUIEventAdapter::KEY_A):
                {
                    obj.add_vel(osg::Vec3d(-movespeed,0.0,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Right):
                case(osgGA::GUIEventAdapter::KEY_D):
                {
                    obj.add_vel(osg::Vec3d(movespeed,0.0,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_S):
                case(osgGA::GUIEventAdapter::KEY_Down):
                {
                    obj.add_vel(osg::Vec3d(0.0,0.0,movespeed));
                }break;
                case(osgGA::GUIEventAdapter::KEY_W):
                case(osgGA::GUIEventAdapter::KEY_Up):
                {
                    obj.add_vel(osg::Vec3d(0.0,0.0,-movespeed));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Q):
                {
                    obj.add_rot(osg::Quat(0.001,osg::Vec3d(0.0,1.0,0.0)));
                }break;
                case(osgGA::GUIEventAdapter::KEY_E):
                {
                    obj.add_rot(osg::Quat(-0.001,osg::Vec3d(0.0,1.0,0.0)));
                }break;
                case(osgGA::GUIEventAdapter::KEY_G):
                {
                    obj.add_vel(osg::Vec3d(0.0,0.0,-0.398978678676678688));
                }break;
                default:
                    break;
            }
        }
    }
    virtual ~KeyboardEventHandler() {};
};

typedef boost::multi_array<int, 2> maze_type;
typedef boost::multi_array<osg::ref_ptr<osg::PositionAttitudeTransform>, 2> maze_xform_type;

#define TAU (M_PI * 2)


int main( int , char ** )
{
    osg::Group* root = new osg::Group;

    //osgDB::writeNodeFile(*root,"geoemtry.osgt");
    auto box = osgDB::readRefNodeFile("resources/box.IVE");
    auto guy = osgDB::readRefNodeFile("resources/WIPguy.IVE");
    auto mazexform = osg::ref_ptr<osg::MatrixTransform>(new osg::MatrixTransform());
    mazexform->setMatrix(osg::Matrixd::rotate(osg::Vec3d(0,-1,0), osg::Vec3d(0,0,-1)));
//    root->addChild(mazexform);

    auto guymodelxform = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform());
    guymodelxform->setAttitude(osg::Quat(-TAU/4, osg::Vec3d(1,0,0)) * osg::Quat(TAU/2, osg::Vec3d(0,1,0)));
    guymodelxform->setScale(osg::Vec3d(0.05,0.05,0.05));

    auto guyxform = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform());
    guyxform->addChild(guymodelxform);

    const int map_size_h = 81;
    const int map_size_w = 91;
    const double difficulty = 1;

    auto maze = maps::Maze(map_size_w, map_size_h, difficulty);

    // draw maze
    maze_xform_type maze_xforms(boost::extents[maze.getHeight()][maze.getWidth()]);
    for (size_t i = 0; i < maze.getHeight(); i++){
        for (size_t j = 0; j < maze.getWidth(); j++){
            if (maze.isWall(j, i)){
                maze_xforms[i][j] = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform);
                maze_xforms[i][j]->addChild(box);
                maze_xforms[i][j]->setPosition(osg::Vec3d(j*40, i*40, 0));
                maze_xforms[i][j]->setAttitude(osg::Quat(M_PI, osg::Vec3d(1,0,0) ));
                mazexform->addChild(maze_xforms[i][j]);
            }
        }
    }

    guymodelxform->addChild(guy);

    root->addChild( guyxform );

    osgViewer::Viewer viewer;
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    PhysicsObject obj(guyxform);
    guyxform->setPosition(osg::Vec3d(1,1,0));

    root->addChild(mazexform);

    // add model to viewer.
    viewer.setSceneData( root );

    auto eventHandler = osg::ref_ptr<KeyboardEventHandler>(new KeyboardEventHandler());
    // add the stats handler
//    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(eventHandler);

    viewer.realize();
    viewer.setCameraManipulator( nullptr );
//    viewer.getCameraManipulator()->setHomePosition(
//            osg::Vec3d(1111,111101,10),
//            osg::Vec3d(11110,1110,-1),
//            osg::Vec3d(11111110,+111111,11110)
//            );
    viewer.getCamera()->setProjectionMatrix(
            osg::Matrixd::perspective(72, 16.0/10.0, 1, 1000)
            * osg::Matrixd::lookAt(
                osg::Vec3d(0,0,0),
                osg::Vec3d(0,0,-1),
                osg::Vec3d(0,1,0)));
//    viewer.run();
    while (!viewer.done()) {
        eventHandler->apply_commands(obj);
        obj.move();
        viewer.getCamera()->setViewMatrix(
                osg::Matrixd::lookAt(
                    /* where we are */ obj.xform->getPosition() + obj.xform->getAttitude() * osg::Vec3d(0,6,6),
                    /* what we are looking at */ obj.xform->getPosition(),
                    /* up vector */ osg::Vec3d(0,1,0)
                    ));
        obj.clear();
        viewer.frame();
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
