#include "ComHelper.hpp"
#include "AppxPackaging.hpp"

namespace xPlat {
    class AppxFactory : public xPlat::ComClass<AppxFactory, IAppxFactory>
    {
    public:
        AppxFactory(APPX_VALIDATION_OPTION validationOptions, COTASKMEMALLOC* memalloc, COTASKMEMFREE* memfree ) : 
            m_validationOptions(validationOptions), m_memalloc(memalloc), m_memfree(memfree)
        {
            ThrowErrorIf(Error::InvalidParameter, (m_memalloc == nullptr || m_memfree == nullptr), "allocator/deallocator pair not specified.")
        }

        // IAppxFactory
        HRESULT STDMETHODCALLTYPE CreatePackageWriter(
            IStream* outputStream,
            APPX_PACKAGE_SETTINGS* ,//settings, TODO: plumb this through
            IAppxPackageWriter**packageWriter)
        {
            return xPlat::ResultOf([&]() {
                return xPlat::ResultOf([&]() {
                    ThrowErrorIfNot(Error::InvalidParameter, (packageReader), "Invalid parameter");
                    ComPtr<IAppxPackageWriter> reader(new appx(
                        m_validationOptions,
                        std::move(std::make_unique<xPlat::ZipObject>(inputStream))
                        ));
                    *packageReader = reader.Detach();
                });
            });
        }

        HRESULT STDMETHODCALLTYPE CreatePackageReader(
            IStream* inputStream,
            IAppxPackageReader** packageReader)
        {
            return xPlat::ResultOf([&]() {
                ThrowErrorIfNot(Error::InvalidParameter, (packageReader), "Invalid parameter");
                ComPtr<IAppxPackageReader> reader(new appx(
                    m_validationOptions,
                    std::move(std::make_unique<xPlat::ZipObject>(inputStream))
                    ));
                *packageReader = reader.Detach();
            });
        }

        HRESULT STDMETHODCALLTYPE CreateManifestReader(
            IStream* inputStream,
            IAppxManifestReader** manifestReader)
        {
            return xPlat::ResultOf([&]() {
                // TODO: Implement
                throw Exception(Error::NotImplemented);
            });
        }

        HRESULT STDMETHODCALLTYPE CreateBlockMapReader(
            IStream* inputStream,
            IAppxBlockMapReader** blockMapReader)
        {
            return xPlat::ResultOf([&]() {
                // TODO: Implement
                throw Exception(Error::NotImplemented);
            });
        }

        HRESULT STDMETHODCALLTYPE CreateValidatedBlockMapReader(
            IStream* blockMapStream,
            LPCWSTR signatureFileName,
            IAppxBlockMapReader** blockMapReader)
        {
            return xPlat::ResultOf([&]() {
                // TODO: Implement
                throw Exception(Error::NotImplemented);
            });
        }

        HRESULT MarshalOutString(std::string& internal, LPWSTR *result)
        {
            return xPlat::ResultOf([&]() {
                ThrowErrorIf(Error::InvalidParameter, (result == nullptr || *result != nullptr), "bad pointer" );                
                auto intermediate = utf8_to_utf16(internal);
                std::size_t countBytes = sizeof(wchar_t)*(internal.size()+1);
                *result = m_memalloc(countBytes);
                ThrowErrorIfNot(Error::OutOfMemory, (*result), "Allocation failed!");
                std::memset(reinterpret_cast<void*>(*result), 0, countBytes);
                std::memcpy(reinterpret_cast<void*>(*result),
                            reinterpret_cast<void*>(intermediate.c_str()),
                            countBytes - sizeof(wchar_t));
            });
        }

        COTASKMEMALLOC* m_memalloc;
        COTASKMEMFREE*  m_memfree;
        APPX_VALIDATION_OPTION m_validationOptions;
    };
}