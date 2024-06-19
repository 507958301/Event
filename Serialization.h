#include <QByteArray>
#include <QDebug>
#include <tuple>
#include <type_traits>
#include <stdexcept> // 添加异常处理需要的头文件

// 通用序列化模板函数（改进版）
template <typename T>
void serialize(QByteArray& out, const T& data)
{
    out.append(reinterpret_cast<const char*>(&data), sizeof(T));
}

// 通用反序列化模板函数（改进版）
template <typename T>
void deserialize(QByteArray& in, T& data)
{
    if (in.size() < sizeof(T)) {
        throw std::runtime_error("Insufficient data for deserialization");
    }
    const char* raw = in.constData();
    data = *reinterpret_cast<const T*>(raw);
    in = in.mid(sizeof(T));
}

// QByteArray 的序列化特化
template <>
void serialize<QByteArray>(QByteArray& out, const QByteArray& data)
{
    quint32 size = data.size();
    serialize(out, size); // 先写入 QByteArray 的大小
    out.append(data);     // 然后写入 QByteArray 的实际数据
}

// QByteArray 的反序列化特化
template <>
void deserialize<QByteArray>(QByteArray& in, QByteArray& data)
{
    //quint32 size;
    //deserialize(in, size); // 先读取 QByteArray 的大小
    //if (static_cast<qint64>(in.size()) < static_cast<qint64>(size)) {
    //    throw std::runtime_error("Insufficient data for QByteArray deserialization");
    //}
    //data = in.left(size); // 从输入的 QByteArray 中取出数据
    //in = in.mid(size);    // 调整输入 QByteArray 的位置
    data = in;
    in = in.mid(in.size());
}

// QString 的序列化特化
template <>
void serialize<QString>(QByteArray& out, const QString& data)
{
    QByteArray utf8Data = data.toUtf8();
    quint32 size = utf8Data.size();
    serialize(out, size); // 先写入 QString 的 UTF-8 数据的大小
    out.append(utf8Data); // 然后写入 UTF-8 数据
}

// QString 的反序列化特化
template <>
void deserialize<QString>(QByteArray& in, QString& data)
{
    quint32 size;
    deserialize(in, size); // 先读取 UTF-8 数据的大小
    if (static_cast<qint64>(in.size()) < static_cast<qint64>(size)) {
        throw std::runtime_error("Insufficient data for QString deserialization");
    }
    QByteArray utf8Data = in.left(size);    // 从输入的 QByteArray 中取出 UTF-8 数据
    data = QString::fromUtf8(utf8Data);     // 转换为 QString
    in = in.mid(size);                      // 调整输入 QByteArray 的位置
}

// 递归序列化结构体成员函数
template <typename Tuple, std::size_t Index = 0>
typename std::enable_if<Index == std::tuple_size<Tuple>::value>::type
serializeStruct(QByteArray&, const Tuple&)
{
    // 递归结束条件
}

template <typename Tuple, std::size_t Index = 0>
typename std::enable_if<Index < std::tuple_size<Tuple>::value>::type
serializeStruct(QByteArray& out, const Tuple& t)
{
    serialize(out, std::get<Index>(t)); // 序列化当前成员
    serializeStruct<Tuple, Index + 1>(out, t); // 递归序列化下一个成员
}

// 递归反序列化结构体成员函数
template <typename Tuple, std::size_t Index = 0>
typename std::enable_if<Index == std::tuple_size<Tuple>::value>::type
deserializeStruct(QByteArray&, Tuple&)
{
    // 递归结束条件
}

template <typename Tuple, std::size_t Index = 0>
typename std::enable_if<Index < std::tuple_size<Tuple>::value>::type
deserializeStruct(QByteArray& in, Tuple& t)
{
    deserialize(in, std::get<Index>(t)); // 反序列化当前成员
    deserializeStruct<Tuple, Index + 1>(in, t); // 递归反序列化下一个成员
}

// 通过宏自动生成序列化和反序列化函数
#define GENERATE_SERIALIZATION(...) \
    QByteArray toByteArray() const  \
    { \
        QByteArray byteArray; \
        auto members = std::tie(__VA_ARGS__); \
        serializeStruct(byteArray, members); /* 调用递归序列化函数 */\
        return byteArray; \
    } \
    void fromByteArray(const QByteArray& byData) \
    { \
        QByteArray copyData = byData; \
        auto members = std::tie(__VA_ARGS__); \
        try { \
            deserializeStruct(copyData, members); /*  调用递归反序列化函数*/ \
        } catch (const std::exception& e) { \
            qDebug() << "Deserialization failed:" << e.what(); \
        } \
    }
