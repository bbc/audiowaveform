rpm:
	mkdir -p SOURCES
	spectool --get-files --directory SOURCES/ SPECS/audiowaveform.spec
	rpmbuild --define "_topdir %(pwd)" --define "_version %(cat ../../VERSION)" -bb SPECS/audiowaveform.spec

clean:
	rm -rf BUILD BUILDROOT RPMS SRPMS
