#include <string>

#include <SFML/Graphics.hpp>

using std::cout;
using std::endl;
using std::string;

namespace Game {

  // Pixels per second^2
  const sf::Vector2f gravity = sf::Vector2f(0., 2000.f);

  const float margin = 50.f;

  class Character {
  public:

    Character ( const sf::IntRect& bbox,
                const string& assetBasePath,
                const string& pathToSprite,
                const string& name,
                bool reversed = false )
      : bbox(bbox)
      , bLeft(bbox.left)
      , bTop(bbox.top)
      , bWidth(bbox.width)
      , bHeight(bbox.height)
      , name(name)
      , nLives(3)
      , velocity(0.f, 0.f)
      , ground(950.f)
      , isJumping(false)
      , reversed(reversed)
    {
      if ( !texture.loadFromFile(pathToSprite) ) {
        cout << "Error loading Character texture" << endl;
      }

      if ( !heartTexture.loadFromFile(assetBasePath + "heart.png") ) {
        cout << "Error loading Heart texture" << endl;
      }

      // Sprite initialization ----------------------------------------------------------------------

      sprite.setTexture(texture);
      sf::IntRect rect = sprite.getTextureRect();
      cWidth = rect.width;
      cHeight = rect.height;

      if ( reversed ) {
        sprite.setTextureRect(sf::IntRect(rect.width, 0, -rect.width, rect.height));
        position = sf::Vector2f(bLeft + bWidth - rect.width - 100.f, ground - rect.height);
      } else {
        position = sf::Vector2f(bLeft + 100.f, ground - rect.height);
      }

      sprite.setPosition(position);

      heartTexture.setRepeated(true);
      heartSprite.setTexture(heartTexture);

      // Game elements ------------------------------------------------------------------------------

      nameText.setString(name);
      nameText.setCharacterSize(63);
      nameText.setPosition(sf::Vector2f(bLeft + margin, bTop + margin));

      sf::Vector2u heartSize = heartTexture.getSize();
      heartSprite.setTextureRect(sf::IntRect(0, 0, nLives * heartSize.x, heartSize.y));
      heartSprite.setPosition(sf::Vector2f(bLeft + margin,
                                           bTop + nameText.getLocalBounds().height + 3 * margin));

    }

    void setFont( const sf::Font& font ) {
      nameText.setFont(font);
    }

    void update( float elapsedTime ) {
      if ( isJumping ) velocity += elapsedTime * gravity;
      position += elapsedTime * velocity;

      // Snap character back
      if ( position.y + cHeight > ground ) {
        position.y = ground - cHeight;
        velocity = sf::Vector2f(0.f, 0.f);
        isJumping = false;
      }

      sprite.setPosition(position);
    }

    void jump() {
      if ( isJumping ) return;

      isJumping = true;
      cout << "Character.jump : jumping" << endl;
      velocity = sf::Vector2f(0., -1000.f);
    }

    const sf::Sprite& getSprite() {
      return sprite;
    }

    void draw( std::shared_ptr<sf::RenderWindow> window ) {
      window->draw(sprite);
      window->draw(nameText);
      window->draw(heartSprite);
    }

  private:

    // Container size
    sf::IntRect bbox;
    int bLeft;
    int bTop;
    int bWidth;
    int bHeight;

    // Title
    const string name;
    sf::Text nameText;

    // Health
    int nLives;
    sf::Texture heartTexture;
    sf::Sprite heartSprite;

    // Character size
    int cWidth;
    int cHeight;

    sf::Vector2f position;
    bool reversed;

    // Kinematics
    sf::Vector2f velocity;

    // Jump mechanics
    bool isJumping;
    float ground;

    sf::Texture texture;
    sf::Sprite  sprite;

  };


};
