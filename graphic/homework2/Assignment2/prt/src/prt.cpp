#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/ray.h>
#include <filesystem/resolver.h>
#include <sh/spherical_harmonics.h>
#include <sh/default_image.h>
#include <Eigen/Core>
#include <fstream>
#include <random>
#include <stb_image.h>

NORI_NAMESPACE_BEGIN

namespace ProjEnv
{
    std::vector<std::unique_ptr<float[]>>
    LoadCubemapImages(const std::string &cubemapDir, int &width, int &height,
                      int &channel)
    {
        std::vector<std::string> cubemapNames{"negx.jpg", "posx.jpg", "posy.jpg",
                                              "negy.jpg", "posz.jpg", "negz.jpg"};
        std::vector<std::unique_ptr<float[]>> images(6);
        for (int i = 0; i < 6; i++)
        {
            std::string filename = cubemapDir + "/" + cubemapNames[i];
            int w, h, c;
            float *image = stbi_loadf(filename.c_str(), &w, &h, &c, 3);
            if (!image)
            {
                std::cout << "Failed to load image: " << filename << std::endl;
                exit(-1);
            }
            if (i == 0)
            {
                width = w;
                height = h;
                channel = c;
            }
            else if (w != width || h != height || c != channel)
            {
                std::cout << "Dismatch resolution for 6 images in cubemap" << std::endl;
                exit(-1);
            }
            images[i] = std::unique_ptr<float[]>(image);
            int index = (0 * 128 + 0) * channel;
            // std::cout << images[i][index + 0] << "\t" << images[i][index + 1] << "\t"
            //           << images[i][index + 2] << std::endl;
        }
        return images;
    }

    const Eigen::Vector3d cubemapFaceDirections[6][3] = {
        {{0, 0, 1}, {0, -1, 0}, {-1, 0, 0}},  // negx
        {{0, 0, 1}, {0, -1, 0}, {1, 0, 0}},   // posx
        {{1, 0, 0}, {0, 0, -1}, {0, -1, 0}},  // negy
        {{1, 0, 0}, {0, 0, 1}, {0, 1, 0}},    // posy
        {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}}, // negz
        {{1, 0, 0}, {0, -1, 0}, {0, 0, 1}},   // posz
    };

    float CalcPreArea(const float &x, const float &y)
    {
        return std::atan2(x * y, std::sqrt(x * x + y * y + 1.0));
    }

    float CalcArea(const float &u_, const float &v_, const int &width,
                   const int &height)
    {
        // transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]   从 0 到 width  转换到  -1到1
        // ( 0.5 is for texel center addressing)
        float u = (2.0 * (u_ + 0.5) / width) - 1.0;
        float v = (2.0 * (v_ + 0.5) / height) - 1.0;

        // shift from a demi texel, mean 1.0 / size  with u and v in [-1..1]
        float invResolutionW = 1.0 / width;
        float invResolutionH = 1.0 / height;

        // u and v are the -1..1 texture coordinate on the current face.
        // get projected area for this texel
        float x0 = u - invResolutionW;
        float y0 = v - invResolutionH;
        float x1 = u + invResolutionW;
        float y1 = v + invResolutionH;
        float angle = CalcPreArea(x0, y0) - CalcPreArea(x0, y1) -
                      CalcPreArea(x1, y0) + CalcPreArea(x1, y1);

        return angle;
    }

    // template <typename T> T ProjectSH() {}

    template <size_t SHOrder>
    std::vector<Eigen::Array3f> PrecomputeCubemapSH(const std::vector<std::unique_ptr<float[]>> &images,
                                                    const int &width, const int &height,
                                                    const int &channel)
    {
        std::vector<Eigen::Vector3d> cubemapDirs;                                                                       //nomal vector of every pixel 's center
        cubemapDirs.reserve(6 * width * height);
        for (int i = 0; i < 6; i++)                                                                                     //compute nomal vector
        {
            Eigen::Vector3d faceDirX = cubemapFaceDirections[i][0];
            Eigen::Vector3d faceDirY = cubemapFaceDirections[i][1];
            Eigen::Vector3d faceDirZ = cubemapFaceDirections[i][2];
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    double u = 2 * ((x + 0.5) / width) - 1;
                    double v = 2 * ((y + 0.5) / height) - 1;
                    Eigen::Vector3d dir = (faceDirX * u + faceDirY * v + faceDirZ).normalized();
                    cubemapDirs.push_back(dir);
                }
            }
        }
        constexpr int SHNum = (SHOrder + 1) * (SHOrder + 1);                                                            //counts of SHCoeffiecents in SH base fuction 
        std::vector<Eigen::Array3f> SHCoeffiecents(SHNum);
        for (int i = 0; i < SHNum; i++)
            SHCoeffiecents[i] = Eigen::Array3f(0);
        float sumWeight = 0;
        float delta_wi = 0.f;
        std::cout << "start precompute environment lights..." << std::endl;
        for (int i = 0; i < 6; i++)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // TODO: here you need to compute light sh of each face of cubemap of each pixel
                    // TODO: 此处你需要计算每个像素下cubemap某个面的球谐系数
                    Eigen::Vector3d dir = cubemapDirs[i * width * height + y * width + x];                              //the piexl's center
                    int index = (y * width + x) * channel;
                    Eigen::Array3f Le(images[i][index + 0], images[i][index + 1], images[i][index + 2]);                //this piexl's value
                    //Le = Eigen::Array3f(0.5f);                                                                        //test precompute product
                    delta_wi = CalcArea(x, y, width, height) / (4.f * M_PI);
                    for (int l = 0; l <= SHOrder; l++) {
                        for (int m = -l; m < l+1; m++) {
                            int i = sh::GetIndex(l, m);
                            double sh = sh::EvalSH(l, m, dir);
                            SHCoeffiecents[i] += (float)sh * delta_wi * Le.array();
                        }
                    }
                }
            }
        }
        std::cout << "end precompute environment lights..." << std::endl;
        return SHCoeffiecents;
    }
}

class PRTIntegrator : public Integrator
{
public:
    static constexpr int SHOrder = 2;
    static constexpr int SHCoeffLength = (SHOrder + 1) * (SHOrder + 1);

    enum class Type
    {
        Unshadowed = 0,
        Shadowed = 1,
        Interreflection = 2
    };

    PRTIntegrator(const PropertyList &props)
    {
        /* No parameters this time */
        m_SampleCount = props.getInteger("PRTSampleCount", 100);
        m_CubemapPath = props.getString("cubemap");
        auto type = props.getString("type", "unshadowed");
        if (type == "unshadowed")
        {
            m_Type = Type::Unshadowed;
        }
        else if (type == "shadowed")
        {
            m_Type = Type::Shadowed;
        }
        else if (type == "interreflection")
        {
            m_Type = Type::Interreflection;
            m_Bounce = props.getInteger("bounce", 1);
        }
        else
        {
            throw NoriException("Unsupported type: %s.", type);
        }
    }

    virtual void preprocess(const Scene *scene) override
    {

        // Here only compute one mesh
        const auto mesh = scene->getMeshes()[0];
        // Projection environment
        auto cubePath = getFileResolver()->resolve(m_CubemapPath);
        auto lightPath = cubePath / "light.txt";
        auto transPath = cubePath / "transport.txt";
        std::ofstream lightFout(lightPath.str());
        std::ofstream fout(transPath.str());
        int width, height, channel;
        std::vector<std::unique_ptr<float[]>> images =
            ProjEnv::LoadCubemapImages(cubePath.str(), width, height, channel);
        auto envCoeffs = ProjEnv::PrecomputeCubemapSH<SHOrder>(images, width, height, channel);
        m_LightCoeffs.resize(3, SHCoeffLength);
        for (int i = 0; i < envCoeffs.size(); i++)
        {
            lightFout << (envCoeffs)[i].x() << " " << (envCoeffs)[i].y() << " " << (envCoeffs)[i].z() << std::endl;
            m_LightCoeffs.col(i) = (envCoeffs)[i];
        }
        std::cout << "Computed light sh coeffs from: " << cubePath.str() << " to: " << lightPath.str() << std::endl;
        // Projection transport
        m_TransportSHCoeffs.resize(SHCoeffLength, mesh->getVertexCount());
        fout << mesh->getVertexCount() << std::endl;
        for (int i = 0; i < mesh->getVertexCount(); i++)
        {
            std::cout << "directly:" << i << "/" << mesh->getVertexCount() << std::endl;
            const Point3f &v = mesh->getVertexPositions().col(i);
            const Normal3f &n = mesh->getVertexNormals().col(i);
            auto shFunc = [&](double phi, double theta) -> double {
                Eigen::Array3d d = sh::ToVector(phi, theta);
                const auto wi = Vector3f(d.x(), d.y(), d.z());
                if (m_Type == Type::Unshadowed)
                {
                    // TODO: here you need to calculate unshadowed transport term of a given direction
                    return std::max((double)wi.dot(n),(double)0);
                }
                else
                {
                    // TODO: here you need to calculate shadowed transport term of a given direction
                    nori::Ray3f shadowray = nori::Ray3f(v,wi);
                    bool isIntersect = scene->rayIntersect(shadowray);
                    return isIntersect?0: std::max((double)wi.dot(n), (double)0);
                }
                return 0;
            };
            auto shCoeff = sh::ProjectFunction(SHOrder, shFunc, m_SampleCount); // base fuction's Coeff fit
            for (int j = 0; j < shCoeff->size(); j++)
            {

                m_TransportSHCoeffs.col(i).coeffRef(j) = (*shCoeff)[j];
            }
        }
        if (m_Type == Type::Interreflection)
        {
            // TODO: leave for bonus
            for (int i = 0; i < mesh->getVertexCount(); i++) {
                std::cout << "indirectly:" << i << "/" << mesh->getVertexCount() << std::endl;
                const Point3f& v = mesh->getVertexPositions().col(i);
                const Normal3f& n = mesh->getVertexNormals().col(i);
                auto indirectShFuc = [&](double phi, double theta) ->double {
                    Eigen::Array3d d = sh::ToVector(phi, theta);
                    const auto wi = Vector3f(d.x(),d.y(),d.z());
                    nori::Ray3f inray = nori::Ray3f(v, wi);
                    nori::Intersection its;
                    if (!scene->rayIntersect(inray, its)) return 0;
                    //get transport SHCoeffs of 3 vertex of the trangle 
                    const Eigen::Matrix<Vector3f::Scalar, SHCoeffLength, 1> sh0 = m_TransportSHCoeffs.col(its.tri_index.x()),
                        sh1 = m_TransportSHCoeffs.col(its.tri_index.y()),
                        sh2 = m_TransportSHCoeffs.col(its.tri_index.z());
                    //get light SHCoeffs
                    const Eigen::Matrix<Vector3f::Scalar, SHCoeffLength, 1> rL = m_LightCoeffs.row(0), gL = m_LightCoeffs.row(1), bL = m_LightCoeffs.row(2);
                    //get color of 3 vertex of the trangle
                    Color3f c0 = Color3f(rL.dot(sh0), gL.dot(sh0), bL.dot(sh0)),
                        c1 = Color3f(rL.dot(sh1), gL.dot(sh1), bL.dot(sh1)),
                        c2 = Color3f(rL.dot(sh2), gL.dot(sh2), bL.dot(sh2));
                    //get the intersection color (bary)
                    const Vector3f& bary = its.bary;
                    Color3f c = bary.x() * c0 + bary.y() * c1 + bary.z() * c2;
                    double irradance = (c.r() + c.g() + c.b()) / 3.;
                    return irradance * std::max((double)wi.dot(n), (double)0);
                };
                auto indirectShCoeff = sh::ProjectFunction(SHOrder, indirectShFuc, m_SampleCount);
                for (int j = 0; j < indirectShCoeff->size(); j++) {
                    m_TransportSHCoeffs.col(i).coeffRef(j) += (*indirectShCoeff)[j];
                }
            }
        }

        // Save in face format
        for (int f = 0; f < mesh->getTriangleCount(); f++)
        {
            const MatrixXu &F = mesh->getIndices();
            uint32_t idx0 = F(0, f), idx1 = F(1, f), idx2 = F(2, f);
            for (int j = 0; j < SHCoeffLength; j++)
            {
                fout << m_TransportSHCoeffs.col(idx0).coeff(j) << " ";
            }
            fout << std::endl;
            for (int j = 0; j < SHCoeffLength; j++)
            {
                fout << m_TransportSHCoeffs.col(idx1).coeff(j) << " ";
            }
            fout << std::endl;
            for (int j = 0; j < SHCoeffLength; j++)
            {
                fout << m_TransportSHCoeffs.col(idx2).coeff(j) << " ";
            }
            fout << std::endl;
        }
        std::cout << "Computed SH coeffs"
                  << " to: " << transPath.str() << std::endl;
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const
    {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        const Eigen::Matrix<Vector3f::Scalar, SHCoeffLength, 1> sh0 = m_TransportSHCoeffs.col(its.tri_index.x()),
                                                                sh1 = m_TransportSHCoeffs.col(its.tri_index.y()),
                                                                sh2 = m_TransportSHCoeffs.col(its.tri_index.z());
        const Eigen::Matrix<Vector3f::Scalar, SHCoeffLength, 1> rL = m_LightCoeffs.row(0), gL = m_LightCoeffs.row(1), bL = m_LightCoeffs.row(2);

        Color3f c0 = Color3f(rL.dot(sh0), gL.dot(sh0), bL.dot(sh0)),
                c1 = Color3f(rL.dot(sh1), gL.dot(sh1), bL.dot(sh1)),
                c2 = Color3f(rL.dot(sh2), gL.dot(sh2), bL.dot(sh2));

        const Vector3f &bary = its.bary;
        Color3f c = bary.x() * c0 + bary.y() * c1 + bary.z() * c2;
        // TODO: you need to delete the following four line codes after finishing your calculation to SH,
        //       we use it to visualize the normals of model for debug.
        // TODO: 在完成了球谐系数计算后，你需要删除下列四行，这四行代码的作用是用来可视化模型法线
        if (c.isZero()) {
            auto n_ = its.shFrame.n.cwiseAbs();
            return Color3f(n_.x(), n_.y(), n_.z());
        }
        return c;
    }

    std::string toString() const
    {
        return "PRTIntegrator[]";
    }

private:
    Type m_Type;
    int m_Bounce = 1;
    int m_SampleCount = 100;
    std::string m_CubemapPath;
    Eigen::MatrixXf m_TransportSHCoeffs;
    Eigen::MatrixXf m_LightCoeffs;
};

NORI_REGISTER_CLASS(PRTIntegrator, "prt");
NORI_NAMESPACE_END