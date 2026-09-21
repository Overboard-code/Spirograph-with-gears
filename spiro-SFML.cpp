#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <numeric>
#include <iostream>

// Compilation commands:
// Required SFML installed:  sudo apt install libsfml-dev build-essential
// Linux: g++ -o spirograph spiro-SFML.cpp -lsfml-graphics -lsfml-window -lsfml-system -no-pie
// Windows Cross-Compile: x86_64-w64-mingw32-g++ -o spirograph.exe spiro-SFML.cpp -DSFML_STATIC -I./SFML-2.6.2/include -L./SFML-2.6.2/lib -static -static-libgcc -static-libstdc++ -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -lopengl32 -lwinmm -lgdi32 -luser32 -lkernel32 -luuid -mwindows
//const double PI = 3.14159265358979323846;
 
void drawGearTeeth(sf::RenderWindow& window, sf::Vector2f center, double radius, int numTeeth, float baseRotation, float toothHeight, sf::Color baseColor, bool teethPointOutward = true) {
    float angleStep = M_PI / numTeeth;
    float directionMultiplier = teethPointOutward ? 1.0f : -1.0f;

    for (int i = 0; i < numTeeth * 2; ++i) {
        sf::VertexArray segment(sf::PrimitiveType::LineStrip);
        
        float angleA = baseRotation + (i * angleStep);
        float deltaR_A = (i % 2 != 0) ? (toothHeight / 2.0f) : (-toothHeight / 2.0f);
        float radiusA = static_cast<float>(radius) + (deltaR_A * directionMultiplier);
        float x1 = center.x + radiusA * std::cos(angleA);
        float y1 = center.y + radiusA * std::sin(angleA);

        float angleB = baseRotation + ((i + 1) * angleStep);
        float deltaR_B = ((i + 1) % 2 != 0) ? (toothHeight / 2.0f) : (-toothHeight / 2.0f);
        float radiusB = static_cast<float>(radius) + (deltaR_B * directionMultiplier);
        float x2 = center.x + radiusB * std::cos(angleB);
        float y2 = center.y + radiusB * std::sin(angleB);

        segment.append(sf::Vertex(sf::Vector2f(x1, y1), baseColor));
        segment.append(sf::Vertex(sf::Vector2f(x2, y2), baseColor));
        window.draw(segment);
    }
}
int main() {
    
    const double REAL_PITCH = 10.0;           
    const double RING_WALL_THICKNESS = 76.4;  
    int ringInTeeth = 105; 
	int ringOutTeeth = ringInTeeth + static_cast<int>(std::round((2.0 * M_PI * RING_WALL_THICKNESS) / REAL_PITCH));
    int wheelTeeth = 72;
    float d_fraction = 0.75f; 
    bool rollOutside = false; 
    
    std::vector<sf::Color> colorPalette = { 
        sf::Color(255, 0, 0), sf::Color(255, 127, 0), sf::Color(255, 255, 0), 
        sf::Color(127, 255, 0), sf::Color(0, 255, 0), sf::Color(0, 255, 127), 
        sf::Color(0, 255, 255), sf::Color(0, 127, 255), sf::Color(127, 0, 255), 
        sf::Color(238, 130, 238), sf::Color::White, sf::Color(64, 64, 64) 
    };
    size_t activeColorIdx = 0;
    sf::Color currentPenColor = colorPalette[0]; 
    sf::Color wheelColor = sf::Color(0, 160, 255);
    int transparency = 40;
    const int columnsPerRow = 6;
    const float buttonSize = 35.0f;
    const float spacing = 10.0f;
    const float paletteStartX = 20.0f;
    const float paletteStartY = 450.0f; 

    
    double R, r, d, t = 0.0;       
    double speed = 0.003;  
    int stepsPerFrame = 50; 
    int totalTeethPeriods;

    double maxTrackAngle;
    bool simulationFinished = false;
    bool rainbowMode = false;                 
	
    auto resetEngineMath = [&]() {
        double R_internal = (ringInTeeth * REAL_PITCH) / (2.0 * M_PI);
        r = (wheelTeeth * REAL_PITCH) / (2.0 * M_PI);
        d = r * d_fraction; 
        t = 0.0;
        
        if (!rollOutside) {
            R = R_internal;
            totalTeethPeriods = std::lcm(ringInTeeth, wheelTeeth);
            maxTrackAngle = 2.0 * M_PI * (double(totalTeethPeriods) / double(ringInTeeth));
        } else {
            // Calculate a non-fractional whole number of teeth for the external radius
            ringOutTeeth = ringInTeeth + static_cast<int>(std::round((2.0 * M_PI * RING_WALL_THICKNESS) / REAL_PITCH));
            R = (ringOutTeeth * REAL_PITCH) / (2.0 * M_PI); // Dynamic tracking adjustment
            totalTeethPeriods = std::lcm(ringOutTeeth, wheelTeeth);
            maxTrackAngle = 2.0 * M_PI * (double(totalTeethPeriods) / double(ringOutTeeth));
        }
        simulationFinished = false;
    };
    resetEngineMath();
    const int WINDOW_WIDTH = 1300;
    const int WINDOW_HEIGHT = 850;
    const float SIDEBAR_WIDTH = 350.0f;
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Paul's Spirograph Engine");
    window.setFramerateLimit(60);

    sf::Vector2f spiroCenter((WINDOW_WIDTH + SIDEBAR_WIDTH) / 2.0f, WINDOW_HEIGHT / 2.0f);
    std::vector<sf::Vertex> path;

    sf::RectangleShape sidebarBackground(sf::Vector2f(SIDEBAR_WIDTH, WINDOW_HEIGHT));
    sidebarBackground.setFillColor(sf::Color(25, 25, 35));
    sidebarBackground.setOutlineThickness(2.0f);
    sidebarBackground.setOutlineColor(sf::Color(45, 45, 60));

    sf::Font standardFont;
    // Look for the font natively inside a local project subdirectory
	if (!standardFont.loadFromFile("/usr/share//fonts/truetype/dejavu/DejaVuSans.ttf")) { //Linux?
	   if (!standardFont.loadFromFile("assets/DejaVuSans.ttf")) {
          if (!standardFont.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf")) { // Windows? 
		    std::cout<<"Font not found. Please copy a ttf font to './assets/DejaVuSans.ttf' \n"; 
	      }
	   }
	}


    sf::Text textring, textwheel, textPen, textMode, textSpeed, textPaletteLabel;
    
    auto makeTextLabel = [&](sf::Text& lbl, const sf::Font& f, std::string txt, sf::Vector2f p) {
        lbl.setFont(f);
        lbl.setString(txt);
        lbl.setCharacterSize(14);
        lbl.setFillColor(sf::Color(210, 210, 230));
        lbl.setPosition(p);
    };
    int o_TeethCount = ringInTeeth + static_cast<int>((2.0 * M_PI * RING_WALL_THICKNESS) / REAL_PITCH);
    makeTextLabel(textring, standardFont, "Ring Teeth " + std::to_string(ringInTeeth) + "/" + std::to_string(o_TeethCount), sf::Vector2f(20, 70));
    makeTextLabel(textwheel, standardFont, "Wheel Teeth " + std::to_string(wheelTeeth), sf::Vector2f(20, 170));
    makeTextLabel(textPen, standardFont, "Pen Distance " + std::to_string(static_cast<int>(d_fraction * 100)) + "%", sf::Vector2f(20, 270));
    makeTextLabel(textMode, standardFont, "Gearing Mode: Internal", sf::Vector2f(35, 358));
    makeTextLabel(textPaletteLabel, standardFont, "Active Pen Color:", sf::Vector2f(20, 420));
    makeTextLabel(textSpeed, standardFont, "Steps Per Frame: " + std::to_string(stepsPerFrame) + "\n(Up/Down to shift speed)\n(Space to Clear Screen)", sf::Vector2f(20, 620));

    sf::Clock clickTimer;

    sf::RectangleShape ringSlider(sf::Vector2f(300, 10)); ringSlider.setPosition(20, 105); ringSlider.setFillColor(sf::Color(60,60,80));
    sf::RectangleShape wheelSlider(sf::Vector2f(300, 10)); wheelSlider.setPosition(20, 205); wheelSlider.setFillColor(sf::Color(60,60,80));
    sf::RectangleShape penSlider(sf::Vector2f(300, 10));   penSlider.setPosition(20, 305);   penSlider.setFillColor(sf::Color(60,60,80));

    sf::CircleShape ringHandle(7); ringHandle.setFillColor(sf::Color::White); ringHandle.setOrigin(7, 7);
    sf::CircleShape wheelHandle(7); wheelHandle.setFillColor(sf::Color::White); wheelHandle.setOrigin(7, 7);
    sf::CircleShape penHandle(7);   penHandle.setFillColor(sf::Color::White);   penHandle.setOrigin(7, 7);

    sf::RectangleShape modeButton(sf::Vector2f(300, 35)); modeButton.setPosition(20, 350); modeButton.setFillColor(sf::Color(45, 45, 70));
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    stepsPerFrame += 5; 
                    if (stepsPerFrame > 300) stepsPerFrame = 300;
                    textSpeed.setString("Steps Per Frame: " + std::to_string(stepsPerFrame) + "\n(Up/Down to shift speed)\n(Space to Clear Screen)");
                }
                if (event.key.code == sf::Keyboard::Down) {
                    stepsPerFrame -= 5;
                    if (stepsPerFrame < 0) stepsPerFrame = 0;
                    textSpeed.setString("Steps Per Frame: " + std::to_string(stepsPerFrame) + "\n(Up/Down to shift speed)\n(Space to Clear Screen)");
                }
                if (event.key.code == sf::Keyboard::Space) {
                    path.clear();
                    resetEngineMath();
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mPos = sf::Mouse::getPosition(window);

            if (mPos.x >= 20 && mPos.x <= 320 && mPos.y >= 90 && mPos.y <= 120) {
                ringInTeeth = 20 + static_cast<int>((mPos.x - 20) * (250 - 20) / 300.0f);
				o_TeethCount = ringInTeeth + static_cast<int>((2.0 * M_PI * RING_WALL_THICKNESS) / REAL_PITCH);
                textring.setString("Ring Teeth " + std::to_string(ringInTeeth) + "/" + std::to_string(o_TeethCount));
                path.clear();
                resetEngineMath();
            }
            if (mPos.x >= 20 && mPos.x <= 320 && mPos.y >= 190 && mPos.y <= 220) {
                wheelTeeth = 10 + static_cast<int>((mPos.x - 20) * (200 - 10) / 300.0f);
                textwheel.setString("Wheel Teeth " + std::to_string(wheelTeeth));
                path.clear();
                resetEngineMath();
            }
            if (mPos.x >= 20 && mPos.x <= 320 && mPos.y >= 290 && mPos.y <= 320) {
                d_fraction = (mPos.x - 20) * 2.0f / 300.0f;
                if (d_fraction > 2.0f) d_fraction = 2.0f;
                if (d_fraction < 0.0f) d_fraction = 0.0f;
                textPen.setString("Pen Distance " + std::to_string(static_cast<int>(d_fraction * 100)) + "%");
                path.clear();
                resetEngineMath();
            }
            if (mPos.x >= 20 && mPos.x <= 320 && mPos.y >= 350 && mPos.y <= 385) {
                if (clickTimer.getElapsedTime().asSeconds() > 0.25f) {
                    rollOutside = !rollOutside;
                    textMode.setString("Gearing Mode: " + std::string(rollOutside ? "External" : "Internal"));
                    path.clear();
                    resetEngineMath();
                    clickTimer.restart();
                }
            }

            for (size_t i = 0; i < colorPalette.size(); ++i) {
                int col = i % columnsPerRow;
                int row = i / columnsPerRow;
                float x = paletteStartX + col * (buttonSize + spacing);
                float y = paletteStartY + row * (buttonSize + spacing);
                sf::FloatRect bounds(x, y, buttonSize, buttonSize);
                if (bounds.contains(static_cast<sf::Vector2f>(mPos))) {
                    activeColorIdx = i;
                    currentPenColor = colorPalette[i];
                    rainbowMode = false;
                }
            }

            float rainX = paletteStartX + 0 * (buttonSize + spacing);
            float rainY = paletteStartY + 2 * (buttonSize + spacing);
            sf::FloatRect rainBounds(rainX, rainY, buttonSize, buttonSize);
            if (rainBounds.contains(static_cast<sf::Vector2f>(mPos))) {
                rainbowMode = true;
                activeColorIdx = 999; 
            }
        }
        if (!simulationFinished) {
            for (int step = 0; step < stepsPerFrame; ++step) {
                if (t >= maxTrackAngle) {
                    simulationFinished = true;
                    break;
                }
                double cx, cy;
                if (rollOutside) {
                   cx = spiroCenter.x + (R + r) * std::cos(t) - d * std::cos((R + r) * t / r);
                   cy = spiroCenter.y + (R + r) * std::sin(t) - d * std::sin((R + r) * t / r);
                } else {
                   cx = spiroCenter.x + (R - r) * std::cos(t) + d * std::cos((R - r) * t / r);
                   cy = spiroCenter.y + (R - r) * std::sin(t) - d * std::sin((R - r) * t / r);
                }
                
                sf::Color vertColor = currentPenColor;
                if (rainbowMode) {
                    float hue = static_cast<float>(fmod(t * 57.2958 * 2.0, 360.0));
                    float X = (1.0f - std::abs(fmod(hue / 60.0f, 2.0f) - 1.0f)) * 255.0f;
                    if (hue >= 0 && hue < 60)        vertColor = sf::Color(255, static_cast<uint8_t>(X), 0);
                    else if (hue >= 60 && hue < 120)  vertColor = sf::Color(static_cast<uint8_t>(X), 255, 0);
                    else if (hue >= 120 && hue < 180) vertColor = sf::Color(0, 255, static_cast<uint8_t>(X));
                    else if (hue >= 180 && hue < 240) vertColor = sf::Color(0, static_cast<uint8_t>(X), 255);
                    else if (hue >= 240 && hue < 300) vertColor = sf::Color(static_cast<uint8_t>(X), 0, 255);
                    else                              vertColor = sf::Color(255, 0, static_cast<uint8_t>(X));
                }

                path.push_back(sf::Vertex(sf::Vector2f(static_cast<float>(cx), static_cast<float>(cy)), vertColor));
                t += speed;
            }
        }

        ringHandle.setPosition(20 + ((ringInTeeth - 20) / (250.0f - 20.0f)) * 300.0f, 110);
        wheelHandle.setPosition(20 + ((wheelTeeth - 10) / (200.0f - 10.0f)) * 300.0f, 210);
        penHandle.setPosition(20 + (d_fraction / 2.0f) * 300.0f, 310);

        int percentDone = (maxTrackAngle > 0) ? static_cast<int>((t / maxTrackAngle) * 100.0) : 0;
        if (percentDone > 100) percentDone = 100;
        window.setTitle("Paul's Spirograph Engine - " + std::to_string(percentDone) + "%");

        window.clear(sf::Color(10, 10, 15));

        double R_in = (ringInTeeth * REAL_PITCH) / (2.0 * M_PI);
        double R_out = R_in + RING_WALL_THICKNESS;
        float wheelRadius = static_cast<float>(R_in);
        float ringRadius = static_cast<float>(R_out);
		if (!simulationFinished) { 
		sf::CircleShape ringBody(ringRadius);
			ringBody.setOrigin(ringRadius, ringRadius);
			ringBody.setPosition(spiroCenter);
			ringBody.setFillColor(sf::Color(40, 40, 55, 130)); 
			window.draw(ringBody);

			sf::CircleShape wheelCutout(wheelRadius);
			wheelCutout.setOrigin(wheelRadius, wheelRadius);
			wheelCutout.setPosition(spiroCenter);
			wheelCutout.setFillColor(sf::Color(10, 10, 15)); 
			window.draw(wheelCutout); 
		}	
        if (!path.empty()) {
            window.draw(path.data(), path.size(), sf::LinesStrip);
        }
		if (!simulationFinished) { 
		    
			
			
			double R_in_teeth = (ringInTeeth * REAL_PITCH) / (2.0 * M_PI);
			drawGearTeeth(window, spiroCenter, R_in_teeth, ringInTeeth, 0.0f, 10.0f, sf::Color(70, 70, 90), false);

			int ringOutTeeth = ringInTeeth + static_cast<int>((2.0 * M_PI * RING_WALL_THICKNESS) / REAL_PITCH);
			double R_out_teeth = R_in_teeth + RING_WALL_THICKNESS;
			drawGearTeeth(window, spiroCenter, R_out_teeth, ringOutTeeth, 0.0f, 10.0f, sf::Color(70, 70, 90), true);

			double currentMovingGearAngle = rollOutside ? ((R + r) * t / r) : -((R - r) * t / r);
			double movingGearCenterX = spiroCenter.x + (rollOutside ? (R + r) : (R - r)) * std::cos(t);
			double movingGearCenterY = spiroCenter.y + (rollOutside ? (R + r) : (R - r)) * std::sin(t);
			sf::Vector2f gearCenter(static_cast<float>(movingGearCenterX), static_cast<float>(movingGearCenterY));

			float wheelRadius = static_cast<float>(r);
			sf::CircleShape rollingWheelDisc(wheelRadius);
			rollingWheelDisc.setOrigin(wheelRadius, wheelRadius);
			rollingWheelDisc.setPosition(gearCenter);
			rollingWheelDisc.setFillColor(sf::Color(wheelColor.r,wheelColor.g,wheelColor.b,transparency)); 
			rollingWheelDisc.setOutlineColor(sf::Color::Transparent);
			rollingWheelDisc.setOutlineThickness(1.5f);
			window.draw(rollingWheelDisc);

			drawGearTeeth(window, gearCenter, r, wheelTeeth, currentMovingGearAngle, 10.0f, sf::Color(0, 220, 255), true);

			sf::CircleShape centerPin(3.0f);
			centerPin.setFillColor(sf::Color::White);
			centerPin.setOrigin(3.0f, 3.0f);
			centerPin.setPosition(gearCenter);
			window.draw(centerPin);
			
			if (!path.empty()) {
				sf::VertexArray spokes(sf::Lines, 2);
				spokes[0] = sf::Vertex(gearCenter, sf::Color(255, 255, 255, 80));
				spokes[1] = sf::Vertex(path.back().position, sf::Color(255, 255, 255, 80));
				window.draw(spokes);

				sf::CircleShape penHolePlastic(5.0f);
				penHolePlastic.setOrigin(5.0f, 5.0f);
				penHolePlastic.setPosition(path.back().position);
				penHolePlastic.setFillColor(sf::Color(10, 10, 15)); 
				penHolePlastic.setOutlineColor(sf::Color::White);
				penHolePlastic.setOutlineThickness(1.5f);
				window.draw(penHolePlastic);

				sf::CircleShape activePenTip(2.5f);
				activePenTip.setFillColor(path.back().color); 
				activePenTip.setOrigin(2.5f, 2.5f);
				activePenTip.setPosition(path.back().position);
				window.draw(activePenTip);
			}
		}	
			
        window.draw(sidebarBackground);
        window.draw(textring); window.draw(ringSlider); window.draw(ringHandle);
        window.draw(textwheel); window.draw(wheelSlider); window.draw(wheelHandle);
        window.draw(textPen);   window.draw(penSlider);   window.draw(penHandle);
        window.draw(modeButton); window.draw(textMode);
        window.draw(textPaletteLabel); window.draw(textSpeed);

        for (size_t i = 0; i < colorPalette.size(); ++i) {
            int col = i % columnsPerRow;
            int row = i / columnsPerRow;
            float x = paletteStartX + col * (buttonSize + spacing);
            float y = paletteStartY + row * (buttonSize + spacing);
            sf::RectangleShape colorBtn(sf::Vector2f(buttonSize, buttonSize));
            colorBtn.setPosition(x, y);
            colorBtn.setFillColor(colorPalette[i]);
            if (i == activeColorIdx) {
                colorBtn.setOutlineThickness(2.5f);
                colorBtn.setOutlineColor(sf::Color::White);
            } else {
                colorBtn.setOutlineThickness(1.0f);
                colorBtn.setOutlineColor(sf::Color(60, 60, 70));
            }
            window.draw(colorBtn);
        }
		//
		float rX = paletteStartX + 0 * (buttonSize + spacing);
		float rY = paletteStartY + 2 * (buttonSize + spacing);
		sf::VertexArray rainBtn(sf::PrimitiveType::TriangleStrip, 4);
		rainBtn[0].position = sf::Vector2f(rX, rY);                           // Top-Left
		rainBtn[1].position = sf::Vector2f(rX, rY + buttonSize);              // Bottom-Left
		rainBtn[2].position = sf::Vector2f(rX + buttonSize, rY);              // Top-Right
		rainBtn[3].position = sf::Vector2f(rX + buttonSize, rY + buttonSize); // Bottom-Right
		rainBtn[0].color = sf::Color(148, 0, 211);   // Violet
		rainBtn[1].color = sf::Color(0, 0, 255);     // Blue
		rainBtn[2].color = sf::Color(255, 127, 0);   // Orange
		rainBtn[3].color = sf::Color(255, 0, 0);     // Red
		sf::RectangleShape rainOutline(sf::Vector2f(buttonSize, buttonSize));
		rainOutline.setPosition(rX, rY);
		rainOutline.setFillColor(sf::Color::Transparent); // Keep the center clear so the vertices show through

		if (rainbowMode) {
			rainOutline.setOutlineThickness(2.5f);
			rainOutline.setOutlineColor(sf::Color::White);
		} else {
			rainOutline.setOutlineThickness(1.0f);
			rainOutline.setOutlineColor(sf::Color(80, 40, 80));
		}
		window.draw(rainBtn);
		window.draw(rainOutline);

		// 
        window.display();
    }
    return 0;
}
