compile_gml(GameOfLife.gml GameOfLifeGML.h game_of_life_gml)

set(SOURCES
    Board.cpp
    BoardWidget.cpp
    GameOfLifeGML.h
    main.cpp
)

serenity_bin(GameOfLife)
target_link_libraries(GameOfLife LibGUI)
