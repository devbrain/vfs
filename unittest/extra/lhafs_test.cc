//
// Created by igor on 3/28/24.
//

#include <vector>
#include <set>
#include <utility>
#include <doctest/doctest.h>
#include <vfs/extra/lhafs.hh>
#include <bsw/io/memory_stream_buf.hh>
#include <vfs/archive.hh>

#include "utils/test_data.hh"
#include "utils/sha1.hh"

static std::vector<std::pair<std::string, std::string>> expected = {
	{"c13d2b528545f8397c81ec6e6ba81ebeb20537a2", "Nam19651975/data/libs/arp.library"},
	{"bbcf235ce5bf23633abf1dcf93951badbc705f6f", "Nam19651975/data/libs/info.library"},
	{"468d85b4053e8b7a6d52fbe6357037ddff1772dd", "Nam19651975/data/data/french.txt"},
	{"3b3921f9a94d70163444d85b4d11c2cd6f8c0c31", "Nam19651975/data/data/unit.txt"},
	{"9908fdb93959d0fc5554ac21b31867818462f6a2", "Nam19651975/data/data/targets.txt"},
	{"784b59765ea0650049a80ece43077960d2156407", "Nam19651975/data/data/english.txt"},
	{"0d1e522a1acbf540fd1315880bbe4d613dde0b64", "Nam19651975/data/data/german.txt"},
	{"bdafd69109874424e4d778bc6c2bcacae3a93c4d", "Nam19651975/data/data/italian.txt"},
	{"2eaef057d3663875601c6a14473f50b424ca1467", "Nam19651975/data/vietnamHD"},
	{"1bb292d63127b76429fbe3bf52e9e4dc9de5535d", "Nam19651975/data/vietnam"},
	{"ca60b73bc779d95cdcf23ec5e92b44dc21b1b472", "Nam19651975/data/devs/ramdrive.device"},
	{"b85692e12b2fc7eaa02e4d211b043b5cd6272200", "Nam19651975/data/devs/mountlist"},
	{"1852be077224db91cd507c254181931dab3fb247", "Nam19651975/data/devs/system-configuration"},
	{"4b7e9acd4dca10db3fca508ec89146e0f0456383", "Nam19651975/data/devs/keymaps/gb"},
	{"e28ca98af9bd7b962e7a9a7147b23148b480da34", "Nam19651975/data/mod.nam-theme"},
	{"397f60527521b5d648a34a83f525676ce1e59339", "Nam19651975/data/RunNam"},
	{"e5f9b36c591b33867ba14e540056cf7eafe0b68c", "Nam19651975/data/read.me"},
	{"5a3888043a21eb39c6d5d412eeb2447bb5bb50a5", "Nam19651975/data/Install_hd"},
	{"89b10a3acbbaf495aa9c37bb2920e523ae89a8f1", "Nam19651975/data/s/startup-sequence"},
	{"a8e2ae7b89a4bd1fb1fc3ade425b85c80f6746f0", "Nam19651975/data/pictures/presi-SCRN.iff"},
	{"7f2066ebd2cbbce011a74af7a8921e895be4b9d9", "Nam19651975/data/pictures/newspaper.iff"},
	{"89240e24f3e4a2b020321497eb6795b851635f36", "Nam19651975/data/pictures/advice.iff"},
	{"a37a603fe4db36bfd83691297339d500e9ed0565", "Nam19651975/data/pictures/IV-Corps.iff"},
	{"feef1eb3891be4289dd2333b6bdd6d560405af83", "Nam19651975/data/pictures/book3.iff"},
	{"4a16d52819ee4783a4ebb8f841a26f0e4af734ab", "Nam19651975/data/pictures/ALL-CORPS.iff"},
	{"23f1995473cf992b8d4341dbe41e7bf804c2dcc4", "Nam19651975/data/pictures/presi-Nixon.iff"},
	{"9e2f73a87ec5ab79bcfa1c1d69a0c1724b4b46cf", "Nam19651975/data/pictures/II-Corps.iff"},
	{"f8352dc80b64a55420674c4a18922b0d32f0173a", "Nam19651975/data/pictures/book.iff"},
	{"181f23acce36137b60a65c59ec0838047cb70bdb", "Nam19651975/data/pictures/book1.iff"},
	{"4147622971a05cd6390adb0bc1774bdcf7d60435", "Nam19651975/data/pictures/book2.iff"},
	{"9e32dd379ff2ec9ad2dcc9e69e501a73a01fb5ab", "Nam19651975/data/pictures/namlogo.iff"},
	{"e3b07e14026de9ff6b99f408f3529ca2c19a05c0", "Nam19651975/data/pictures/goodend.iff"},
	{"058d6ca5ca88d004b0046df892fc76f60d1a7447", "Nam19651975/data/pictures/badend.iff"},
	{"4669be86b58b6329f790c654c385a0a6696cb6a2", "Nam19651975/data/pictures/I-Corps.iff"},
	{"e3cddc0e17a026312379d83607f1ebfe0637718c", "Nam19651975/data/pictures/newscut.iff"},
	{"3ad3a81701113017be553fbfdd8982a0242936f6", "Nam19651975/data/pictures/icons.iff"},
	{"f625916e4a84802353acb8b8bc7ed0bb44bf9f69", "Nam19651975/data/pictures/advicenixon.iff"},
	{"896c423b6692fbc2214ec4c6b279560aec1d4382", "Nam19651975/data/pictures/titlepic.iff"},
	{"2c213ac93a9e4e450bb1a681c42eefb042408b0a", "Nam19651975/data/pictures/III-Corps.iff"},
	{"30ede410472723c95338ed2c870916ad275e9706", "Nam19651975/data/l/disk-validator"},
	{"c0b036019fbffc46b417a72dd296f8d33b19a308", "Nam19651975/data/l/ram-handler"},
	{"72601af795153998859fb8a7c2d2c563b0dd6f36", "Nam19651975/data/system/setmap"},
	{"b1cc17a9f68be7bf193df835b7ab3441308aad57", "Nam19651975/data/system/fastmemfirst"},
	{"382984a0cdc9eda1c564de0ca1f46b5f9e987c7e", "Nam19651975/data/c/Endif"},
	{"be967263f84e996684d07bfbb723628bd1527d0e", "Nam19651975/data/c/addbuffers"},
	{"93a5940083bcc1fa52934add7add9e2140d2b8d4", "Nam19651975/data/c/setclock"},
	{"88cb749212949a942b60e656fa91d27f49212359", "Nam19651975/data/c/runback"},
	{"24b9a04d713bdd0080423ffea5ed745ab8ede911", "Nam19651975/data/c/assign"},
	{"a911e7c1fc45413077c2433bd19d92980b6d37da", "Nam19651975/data/c/setpatch"},
	{"47005b3379e550c73b4d173ede239df4a3ee2298", "Nam19651975/data/c/failat"},
	{"48027e677666b82a1c2df9720443a34539f5c6aa", "Nam19651975/data/c/execute"},
	{"8a4b2d83bf6bbb08f22c06992079bbcb53bfd199", "Nam19651975/data/c/run"},
	{"0570069411aadbbed3198eb4786854349e1a86a9", "Nam19651975/data/c/ask"},
	{"1952d5fefe650250080604613f16e91b17db74a3", "Nam19651975/data/c/echo"},
	{"c128595ad2cd96b11c6ff1c2adf9de66053331a6", "Nam19651975/data/c/if"},
	{"eb0f42031af26c7a1c4b4027048a0e498f3376d6", "Nam19651975/data/c/quit"},
	{"f468b3bbc0367a803729119e715ac8ffd777784f", "Nam19651975/data/c/cd"},
	{"14c5e6d253eb03a5621728f27b17c1b01b309510", "Nam19651975/data/c/path"},
	{"0f7eaf913a9a8d50af3573a695a8f4af3b8f0ad6", "Nam19651975/data/c/binddrivers"},
	{"f5267b1a86cd0cf1b2b921ad6a5127169372c976", "Nam19651975/data/c/endcli"},
	{"248db88c1c5e698841de7837a93929628d318dde", "Nam19651975/data/c/makedir"},
	{"54f977b3fe0b872ee32a3e0fcb9486f11e712fbd", "Nam19651975/data/c/copy"},
	{"456d067794a5ba495c1fc1b1f29b1582229fc9f2", "Nam19651975/data/mod.viet-sounds"},
	{"bfaa0ed176b1e4f117646bce2aadf9e8045defc1", "Nam19651975/Nam19651975.slave"},
	{"8bb5d911afee32a466e5c162df5bf900688b28cc", "Nam19651975/Nam19651975.info"},
	{"87f9c449faa3d14b214077ca5d952006caed3c73", "Nam19651975/ReadMe.info"},
	{"edab79f1f3ee05f6266bb2bd12b869927c73d0ab", "Nam19651975/ReadMe"},
	{"44969c4a56d006d7f8113e8990d292d595f49203", "Nam19651975.info"}
};

static std::set<std::string> expected_dirs = {
	"Nam19651975",
	"Nam19651975/data",
	"Nam19651975/data/libs",
	"Nam19651975/data/data",
	"Nam19651975/data/devs",
	"Nam19651975/data/devs/keymaps",
	"Nam19651975/data/s",
	"Nam19651975/data/pictures",
	"Nam19651975/data/l",
	"Nam19651975/data/system",
	"Nam19651975/data/c",
	"Nam19651975/data/fonts"
};

static bool check_dir(const std::string& name) {
	auto real_name = name.substr (1);
	if (expected_dirs.find (real_name) != expected_dirs.end()) {
		return true;
	}
	return false;
}

static bool check_hash(const std::string& hash, const std::string& name) {
	auto n = name.substr (1);
	for (const auto& [c, f] : expected) {
		if (f == n) {
			return hash == c;
		}
	}
	return false;
}

static void traverse(vfs::archive& ar, const std::string& path, std::size_t& files, std::size_t& dirs) {
	auto d = ar.open_directory (path);
	while (d.has_next ()) {
		const auto [name, st] = d.next ();
		auto full_name = path == "/" ? path + name : path + "/" + name;
		if (st.type == vfs::stats::type_t::eFILE) {
			auto* f = ar.open (full_name, vfs::READ_ONLY);
			REQUIRE(f != nullptr);
			files++;
			auto size = st.size;
			char buff[1024];
			std::size_t has_bytes = 0;
			SHA1 hasher;
			while (has_bytes < size) {

				auto rc = vfs::read (f, buff, sizeof (buff));
				REQUIRE(rc > 0);

				has_bytes += rc;
				bsw::io::memory_input_stream mis (buff, rc);
				hasher.update (mis);
			}
			vfs::close (f);
			auto chks_sum = hasher.final ();
			REQUIRE(check_hash (chks_sum, full_name));

		} else if (st.type == vfs::stats::type_t::eDIRECTORY) {
			REQUIRE(check_dir (full_name));
			dirs++;
			traverse (ar, full_name, files, dirs);
		} else {
			REQUIRE(false);
		}
	}
}

TEST_CASE("test lha") {
	vfs::archive ar (vfs::extra::create_lhafs (), get_test_file ("lhafs/test.lha"));

	std::size_t files = 0;
	std::size_t dirs = 0;

	traverse (ar, "/", files, dirs);

	REQUIRE(files == expected.size());
	REQUIRE(dirs == expected_dirs.size());
}
