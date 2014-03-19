#include <cmath>

#include <QFileIconProvider>
#include <QGridLayout>
#include <QPainter>
#include <QSpacerItem>
#include <QTextEdit>
#include <QTransform>

#include <elle/log.hh>

#include <fist-gui-qt/IconButton.hh>
#include <fist-gui-qt/TransactionBody.hh>
#include <fist-gui-qt/TransactionModel.hh>
#include <fist-gui-qt/globals.hh>

ELLE_LOG_COMPONENT("infinit.FIST.TransactionBody");

namespace
{
  class ShrinkingTextEdit:
   public QTextEdit
  {
  public:
    explicit ShrinkingTextEdit(QString const& text,
                               int width = 236):
      QTextEdit(text),
      _width_hint(width)
    {
      this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
      this->setReadOnly(true);
      view::transaction::note::style(*this);
      this->setFrameStyle(0);
      this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      this->updateGeometry();
      this->shrink();
    }

    void
    shrink()
    {
      auto font = this->fontMetrics();
      int left, top, right, bottom;
      getContentsMargins(&left, &top, &right, &bottom);

      // Calculate the number of lines.
      int lines = 1 + ::floor((left + right + font.width(this->toPlainText())) /
                              (1.0f * this->_width_hint));

      int leading = 4; // font.leading() returns 0.
      this->_size = QSize(
        this->_width_hint,
        2 * leading + top + bottom + lines * font.lineSpacing());
    }

    QSize _size;
    int _width_hint;

    QSize
    sizeHint() const override
    {
      return this->_size;
    }

    QSize
    minimumSizeHint() const override
    {
      return this->sizeHint();
    }
  };
}

/*------------.
| Contruction |
`------------*/
TransactionBody::TransactionBody(TransactionModel const& transaction,
                                 Qt::AlignmentFlag alignment,
                                 QWidget* parent):
  QWidget(parent),
  _transaction(transaction),
  _layout(new QGridLayout(this)),
  _alignment(alignment),
  _background(Qt::white),
  _text_progress(nullptr)
{
  //   this->setStyleSheet("background-color:purple;");

  auto layout = this->_layout;

  layout->setContentsMargins(8, 8, 8, 8);
  {
    auto filename = this->_transaction.files().size() == 1 ?
      new QLabel(this->_transaction.files().first()) :
      new QLabel(QString("%1 files").arg(this->_transaction.files().size()));

    IconButton* icon;

    QFileIconProvider icon_provider;
    if (this->_transaction.files().size() > 1 ||
        QFileInfo(filename->text()).isDir())
    {
      icon = new IconButton(icon_provider.icon(QFileIconProvider::Folder).pixmap(18));
      if (this->_transaction.files().size() > 1)
        connect(icon, SIGNAL(clicked()),
                this, SLOT(foo()));
    }
    else
    {
      icon = new IconButton(icon_provider.icon(filename->text()).pixmap(18));
      icon->disable();
    }

    filename->setToolTip(this->_transaction.tooltip());
    view::transaction::files::style(*filename);
    filename->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

    if (this->_alignment == Qt::AlignLeft)
    {
      layout->addWidget(icon, 0, 0, this->_alignment);
      layout->addWidget(filename, 0, 1, this->_alignment);
    }
    else
    {
      layout->addWidget(filename, 0, 0, this->_alignment);
      layout->addWidget(icon, 0, 1, this->_alignment);
    }
  }
  if (!this->_transaction.message().isEmpty())
  {
    layout->addItem(new QSpacerItem(this->sizeHint().width(), 5), 1, 0, 1, -1);
    auto message = new ShrinkingTextEdit(this->_transaction.message());
    // message->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    // message->setReadOnly(true);
    // view::transaction::note::style(*message);


    auto* icon = new QLabel;
    icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    if (this->_alignment == Qt::AlignLeft)
    {
      icon->setPixmap(QString(":/icons/quote.png"));
    }
    else
    {
      icon->setPixmap(QPixmap(QString(":/icons/quote.png")).transformed(QTransform().scale(-1, 1)));
      message->setAlignment(Qt::AlignRight);
    }

    if (this->_alignment == Qt::AlignLeft)
    {
      layout->addWidget(icon, 2, 0, Qt::AlignTop | Qt::AlignCenter);
      layout->addWidget(message, 2, 1, Qt::AlignTop | Qt::AlignRight);
    }
    else
    {
      layout->addWidget(message, 2, 0, Qt::AlignTop | this->_alignment);
      layout->addWidget(icon, 2, 1, Qt::AlignTop | Qt::AlignLeft);
    }
  }
  this->updateGeometry();
}

void
TransactionBody::update_progress()
{
  this->update();
}

void
TransactionBody::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);

  auto border_margin = 0;
  painter.setPen(QColor(0xE8, 0xE8, 0xE8));
  painter.setBrush(this->_background);

  this->updateGeometry();

  QRectF border(border_margin,
                0,
                this->width() - border_margin,
                this->sizeHint().height());
  ELLE_WARN("%s", this->sizeHint().height());
  painter.drawRoundedRect(border, 3, 3);

  if (this->_transaction.status() == gap_transaction_running)
  {
    painter.translate(border_margin + 2, this->height() - 2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0x30, 0xCB, 0xEA));

    float width = this->width() - border_margin - 2 * 2;
    width *= this->_transaction.progress();
    QRectF progress(0, 0, width, 2);
    painter.drawRoundedRect(progress, 1, 1);
  }
}

QSize
TransactionBody::sizeHint() const
{
  return this->_layout->sizeHint();
}

/*----------.
| Printable |
`----------*/
void
TransactionBody::print(std::ostream& stream) const
{
  stream << "TransactionBody(" << this->_transaction << ")";
}
