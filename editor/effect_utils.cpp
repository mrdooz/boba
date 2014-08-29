#include "effect_utils.hpp"

namespace editor
{
  //----------------------------------------------------------------------------------
  RectangleShape CreateRect(const Vector2f& pos, const Vector2f& size, const Color& col)
  {
    RectangleShape shape;
    shape.setPosition(pos);
    shape.setSize(size);
    shape.setFillColor(col);
    return shape;
  }

  //----------------------------------------------------------------------------------
  Text CreateText(const Font& font, const Vector2f& pos, int size, const char* str)
  {
    Text text;
    text.setFont(font);
    text.setPosition(pos);
    text.setCharacterSize(size);
    text.setString(str);
    return text;
  }

  //----------------------------------------------------------------------------------
  void DrawRow(RenderTexture& texture, float x, float y, float w, float h, const Color& color)
  {
    VertexArray curLine(sf::Lines);
    curLine.append(Vertex(Vector2f(x, y), color));
    curLine.append(Vertex(Vector2f(w, y), color));
    curLine.append(Vertex(Vector2f(x, y+h), color));
    curLine.append(Vertex(Vector2f(w, y+h), color));
    texture.draw(curLine);
  }
}
