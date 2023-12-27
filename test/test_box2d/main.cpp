#include <iostream>
#include <Box2D/Box2D.h>

int main() {
    // 创建 Box2D 世界
    b2Vec2 gravity(0.0f, -10.0f);
    b2World world(gravity);

    // 创建地面刚体
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);
    b2Body *groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);

    // 创建动态刚体
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    b2Body *body = world.CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f); // dynamicBox

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef); // fixtureDef

    // 模拟运动（盒子掉落在地上的运动）
    float timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    // for (int32_t i = 0; i < 60; ++i) {
    while (true) {
        world.Step(timeStep, velocityIterations, positionIterations);

        b2Vec2 position = body->GetPosition();
        float_t angle = body->GetAngle();

        std::cout << "位置: (" << position.x << ", " << position.y << ")"
                  << " 角度: " << angle << std::endl;
    }

    return 0;
}
