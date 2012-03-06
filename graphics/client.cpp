/**
 * @file client.cpp
 *
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-07
 */

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
    PhysicsObject(osg::ref_ptr<osg::PositionAttitudeTransform> xform) : xform(xform) {}

    void add_vel(osg::Vec3d v) {
        velocity += v;
    }

    void move() {
        xform->setPosition(xform->getPosition() + velocity*10);
    }
    void clear() {
        velocity = osg::Vec3d(0,0,0);
    }

    osg::Vec3d velocity;
    osg::ref_ptr<osg::PositionAttitudeTransform> xform;
};

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardEventHandler() {}

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
        }
        return true;
    }

    void apply_commands(PhysicsObject& obj) {
        for (auto& key : pressed) {
            switch(key)
            {
                case(osgGA::GUIEventAdapter::KEY_Down):
                {
                    obj.add_vel(osg::Vec3d(0.0,-0.01,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Up):
                {
                    obj.add_vel(osg::Vec3d(0.0,0.01,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Left):
                {
                    obj.add_vel(osg::Vec3d(-0.01,0.0,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Right):
                {
                    obj.add_vel(osg::Vec3d(0.01,0.0,0.0));
                }break;
                case(osgGA::GUIEventAdapter::KEY_C):
                   {
                    obj.add_vel(osg::Vec3d(0.0,0.0,-0.01));
                }break;
                case(osgGA::GUIEventAdapter::KEY_Space):
                {
                    obj.add_vel(osg::Vec3d(0.0,0.0,0.01));
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

int main( int argc, char *argv[] )
{
    osg::Group* root = new osg::Group;

    //osgDB::writeNodeFile(*root,"geoemtry.osgt");
    auto box = osgDB::readRefNodeFile("resources/box.IVE");
    auto scenexform = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform());
    scenexform->setAttitude(osg::Quat(M_PI/3, osg::Vec3d(1,0,0)));
    root->addChild(scenexform);

    auto xform1 = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform());
    int map_size = 51;
    maze_type maze(boost::extents[5][5]);
    maze_xform_type maze_xforms(boost::extents[5][5]);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            maze[i][j] = rand()%2;
            maze_xforms[i][j] = osg::ref_ptr<osg::PositionAttitudeTransform>(new osg::PositionAttitudeTransform);
        }
    }
    int tab_size = 5;
    for (int i = 0; i < tab_size; i++){
        for (int j = 0; j < tab_size; j++){
            if (maze[i][j] == 1){
                 maze_xforms[i][j]->addChild(box);
                 maze_xforms[i][j]->setPosition(osg::Vec3d(j*40, i*40, 0));
                 scenexform->addChild(maze_xforms[i][j]);
            }
        }
    }

    xform1->addChild(box);

    scenexform->addChild( xform1 );

    osgViewer::Viewer viewer;
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    auto xform = root->getChild(0)->asGroup()->getChild(0)->asTransform()
                        ->asPositionAttitudeTransform();
    PhysicsObject obj(xform1);

    // add model to viewer.
    viewer.setSceneData( root );

    auto eventHandler = osg::ref_ptr<KeyboardEventHandler>(new KeyboardEventHandler());
    // add the stats handler
//    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(eventHandler);

    viewer.realize();
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );

//    viewer.run();
    while (!viewer.done()) {
        eventHandler->apply_commands(obj);
        obj.move();
        obj.clear();
        viewer.frame();
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
