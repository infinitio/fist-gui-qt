#include <fist-gui-qt/gui/ImageSerie.hh>

namespace fist
{
  namespace gui
  {
    ImageSerie::ImageSerie(QString const& pattern,
                           uint64_t number,
                           uint64_t offset)
      : _pattern(pattern)
      , _number(number)
      , _offset(offset)
      , _index(offset - 1)
    {
    }

    QPixmap const&
    ImageSerie::next()
    {
      if (this->_number == 0)
      {
        static const QPixmap empty{};
        return empty;
      }
      else if (this->_number == 1 && this->_images.size() == 0)
      {
        this->_images.emplace(0, QPixmap(this->_pattern));
        this->_index = 0;
      }
      else
      {
        this->_index += 1;
        this->_index %= (this->_number + this->_offset);
        if (this->_index == 0)
          this->_index = this->_offset;
        if (this->_images.find(this->_index) == this->_images.end())
          this->_images.emplace(this->_index, QPixmap(this->_pattern.arg(this->_index)));
      }
      return this->_images.at(this->_index);
    }
  }
}
