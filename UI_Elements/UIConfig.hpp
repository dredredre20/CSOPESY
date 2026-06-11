class UIConfig {
public:
    static void initialize();

    static float getScaleFactor();

    static ImVec2 scale(ImVec2 size);

private:
    static float scaleFactor;
};